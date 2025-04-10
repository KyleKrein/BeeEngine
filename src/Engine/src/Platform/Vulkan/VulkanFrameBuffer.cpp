//
// Created by Aleksandr on 10.03.2024.
//

#include "VulkanFrameBuffer.h"

#include "Core/Casts.h"
#include "Core/CodeSafety/Expects.h"
#include "Core/Logging/Log.h"
#include "Core/TypeDefines.h"
#include "Platform/Vulkan/VulkanBuffer.h"
#include "Platform/Vulkan/VulkanImage.h"
#include "Renderer/BindingSet.h"
#include "Renderer/FrameBuffer.h"
#include "Renderer/Renderer.h"
#include "Utils.h"
#include "VulkanTexture2D.h"
#include "backends/imgui_impl_vulkan.h"
#include <cstdint>
#include <vulkan/vulkan_enums.hpp>

namespace BeeEngine::Internal
{
    static uint32_t GetSizeOfPixel(FrameBufferTextureFormat format)
    {
        switch (format)
        {
            case FrameBufferTextureFormat::RGBA8:
                return 4;
            case FrameBufferTextureFormat::RGBA16F:
                return 8;
            case FrameBufferTextureFormat::RedInteger:
                return 4;
            case FrameBufferTextureFormat::Depth24:
                return 4;
            case FrameBufferTextureFormat::None:
            default:
                break;
        }
        BeeCoreError("Unknown FrameBufferTextureFormat");
        return 0;
    }
    vk::Format ConvertToVulkanFormat(FrameBufferTextureFormat format)
    {
        switch (format)
        {
            case FrameBufferTextureFormat::RGBA8:
                return vk::Format::eR8G8B8A8Unorm;
            case FrameBufferTextureFormat::RGBA16F:
                return vk::Format::eR16G16B16A16Sfloat;
            case FrameBufferTextureFormat::RedInteger:
                return vk::Format::eR32Sfloat;
            case FrameBufferTextureFormat::Depth24:
                return vk::Format::eD32Sfloat;
            case FrameBufferTextureFormat::None:
            default:
                break;
        }
        BeeCoreError("Unknown FrameBufferTextureFormat");
        return vk::Format::eUndefined;
    }
    void VulkanFrameBuffer::CreateImageAndImageView(VulkanImage& image,
                                                    vk::ImageView& view,
                                                    FrameBufferTextureFormat format,
                                                    FrameBufferTextureUsage usage)
    {
        vk::ImageCreateInfo imageCreateInfo{};
        imageCreateInfo.imageType = vk::ImageType::e2D;
        imageCreateInfo.extent.width = m_Preferences.Width;
        imageCreateInfo.extent.height = m_Preferences.Height;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.format = ConvertToVulkanFormat(format);
        imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
        imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
        imageCreateInfo.usage = IsDepthFormat(format)
                                    ? vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled
                                    : vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled |
                                          vk::ImageUsageFlagBits::eTransferSrc;
        imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
        imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;

        vk::ImageViewCreateInfo viewCreateInfo{};
        viewCreateInfo.viewType = vk::ImageViewType::e2D;
        viewCreateInfo.format = imageCreateInfo.format;
        viewCreateInfo.subresourceRange.aspectMask =
            IsDepthFormat(format) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;

        vk::MemoryPropertyFlags memoryPropertyFlags = vk::MemoryPropertyFlagBits::eDeviceLocal;

        VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
        m_GraphicsDevice.CreateImageWithInfo(
            imageCreateInfo, viewCreateInfo, memoryPropertyFlags, memoryUsage, image, view);
    }
    VulkanFrameBuffer::VulkanFrameBuffer(const FrameBufferPreferences& preferences)
        : m_Preferences(preferences), m_GraphicsDevice(VulkanGraphicsDevice::GetInstance())
    {
        BEE_PROFILE_FUNCTION();
        for (auto specification : m_Preferences.Attachments.Attachments)
        {
            if (!IsDepthFormat(specification.TextureFormat))
            {
                m_ColorAttachmentSpecification.emplace_back(specification);
            }
            else
            {
                m_DepthAttachmentSpecification = specification;
            }
        }
        // m_CurrentCommandBuffer = m_GraphicsDevice.CreateCommandBuffer();
        VulkanFrameBuffer::Invalidate();
        m_Initiated = true;
    }

    VulkanFrameBuffer::~VulkanFrameBuffer()
    {
        BeeExpects(m_CurrentCommandBuffer == nullptr);
        BeeExpects(m_ColorAttachmentsTextures.size() == m_ColorAttachmentsReadBuffers.size());
        DeletionQueue::Frame().PushFunction(
            [colorTextures = std::move(m_ColorAttachmentsTextures),
             colorReadBuffers = std::move(m_ColorAttachmentsReadBuffers),
             colorSpecs = std::move(m_ColorAttachmentSpecification)]() mutable
            {
                auto& device = VulkanGraphicsDevice::GetInstance();
                for (size_t i = 0; i < colorTextures.size(); ++i)
                {
                    if (colorSpecs[i].TextureUsage == FrameBufferTextureUsage::CPUAndGPU)
                    {
                        device.DestroyBuffer(colorReadBuffers[i].Buffer);
                    }
                }
            });
    }

    CommandBuffer VulkanFrameBuffer::Bind()
    {
        BEE_PROFILE_FUNCTION();
        if (m_Invalid)
        {
            Invalidate();
        }
        ResetReadBuffers();
        m_CurrentCommandBuffer = m_GraphicsDevice.BeginSingleTimeCommands();

        std::vector<vk::RenderingAttachmentInfo> colorAttachments{};
        const auto size = m_ColorAttachmentSpecification.size();

        colorAttachments.reserve(size);

        for (size_t i = 0; i < size; ++i)
        {
            vk::RenderingAttachmentInfo colorAttachment{};
            colorAttachment.imageView = m_ColorAttachmentsTextures[i].GetVulkanImageView();
            colorAttachment.resolveMode = vk::ResolveModeFlagBits::eNone;
            colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
            colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
            colorAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
            colorAttachment.clearValue = m_ColorAttachmentSpecification[i].ClearColor;
            colorAttachments.push_back(colorAttachment);
            m_GraphicsDevice.TransitionImageLayout(
                m_CurrentCommandBuffer,
                m_ColorAttachmentsTextures[i].GetVulkanImage().Image,
                ConvertToVulkanFormat(m_ColorAttachmentSpecification[i].TextureFormat),
                vk::ImageLayout::eShaderReadOnlyOptimal,
                vk::ImageLayout::eColorAttachmentOptimal);
        }

        vk::RenderingAttachmentInfo depthStencilAttachment{};
        depthStencilAttachment.imageView = m_DepthAttachmentTexture->GetVulkanImageView();
        depthStencilAttachment.clearValue.depthStencil =
            vk::ClearDepthStencilValue{m_DepthAttachmentSpecification.ClearDepth, 0};
        depthStencilAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        depthStencilAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        depthStencilAttachment.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::RenderingInfo renderInfo{};
        renderInfo.colorAttachmentCount = colorAttachments.size();
        renderInfo.pColorAttachments = colorAttachments.data();
        renderInfo.layerCount = 1;
        renderInfo.renderArea = vk::Rect2D{{0, 0}, {m_Preferences.Width, m_Preferences.Height}};
        if (m_DepthAttachmentTexture)
        {
            m_GraphicsDevice.TransitionImageLayout(m_CurrentCommandBuffer,
                                                   m_DepthAttachmentTexture->GetVulkanImage().Image,
                                                   ConvertToVulkanFormat(m_DepthAttachmentSpecification.TextureFormat),
                                                   vk::ImageLayout::eUndefined,
                                                   vk::ImageLayout::eDepthStencilAttachmentOptimal);
            renderInfo.pDepthAttachment = &depthStencilAttachment;
        }
        else
        {
            renderInfo.pDepthAttachment = nullptr;
        }

        m_CurrentCommandBuffer.beginRendering(&renderInfo, g_vkDynamicLoader);

        // Установка вьюпорта и сциззора
        vk::Viewport viewport =
            m_GraphicsDevice.CreateVKViewport(m_Preferences.Width, m_Preferences.Height, 0.0f, 1.0f);
        vk::Rect2D scissor = {{0, 0}, {m_Preferences.Width, m_Preferences.Height}};

        m_CurrentCommandBuffer.setViewport(0, 1, &viewport);
        m_CurrentCommandBuffer.setScissor(0, 1, &scissor);

        CommandBuffer commandBuffer{m_CurrentCommandBuffer, &m_RenderingQueue};
        commandBuffer.BeginRecording();
        return commandBuffer;
    }

    void VulkanFrameBuffer::Unbind(CommandBuffer& commandBuffer)
    {
        BeeExpects(m_CurrentCommandBuffer == commandBuffer.GetBufferHandleAs<vk::CommandBuffer>() &&
                   m_CurrentCommandBuffer != nullptr);
        BEE_PROFILE_FUNCTION();
        commandBuffer.EndRecording(); // Flush();
        m_CurrentCommandBuffer.endRendering(g_vkDynamicLoader);
        for (size_t i = 0; i < m_ColorAttachmentsTextures.size(); i++)
        {
            m_GraphicsDevice.TransitionImageLayout(
                m_CurrentCommandBuffer,
                m_ColorAttachmentsTextures[i].GetVulkanImage().Image,
                ConvertToVulkanFormat(m_ColorAttachmentSpecification[i].TextureFormat),
                vk::ImageLayout::eColorAttachmentOptimal,
                vk::ImageLayout::eShaderReadOnlyOptimal);
        }
        m_GraphicsDevice.EndSingleTimeCommands(m_CurrentCommandBuffer);
        // Renderer::SubmitCommandBuffer({m_CurrentCommandBuffer});
        commandBuffer.Invalidate();
        m_CurrentCommandBuffer = nullptr;
    }

    void VulkanFrameBuffer::Resize(uint32_t width, uint32_t height)
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(width > 0 && height > 0 && width < std::numeric_limits<uint32_t>::max() &&
                   height < std::numeric_limits<uint32_t>::max());
        m_Preferences.Width = width;
        m_Preferences.Height = height;
        m_Invalid = true;
    }

    void VulkanFrameBuffer::Invalidate()
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(m_Preferences.Width > 0 && m_Preferences.Height > 0 &&
                   m_Preferences.Width < std::numeric_limits<uint32_t>::max() &&
                   m_Preferences.Height < std::numeric_limits<uint32_t>::max());
        if (m_Initiated)
        {
            m_GraphicsDevice.GetDevice().waitIdle(); // ImGui_ImplVulkan_RemoveTexture fails because descriptor set is
                                                     // in use. And I have no idea, how to overcome this.
            for (size_t i = 0; i < m_ColorAttachmentsTextures.size(); ++i)
            {
                if (m_ColorAttachmentSpecification[i].TextureUsage == FrameBufferTextureUsage::CPUAndGPU)
                {
                    m_GraphicsDevice.DestroyBuffer(m_ColorAttachmentsReadBuffers[i].Buffer);
                }
            }
            m_ColorAttachmentsTextures.clear();
            m_ColorAttachmentsReadBuffers.clear();
            m_DepthAttachmentTexture = nullptr;
        }
        m_ColorAttachmentsTextures.reserve(m_ColorAttachmentSpecification.size());
        m_ColorAttachmentsReadBuffers.resize(m_ColorAttachmentSpecification.size());
        for (size_t i = 0; i < m_ColorAttachmentSpecification.size(); ++i)
        {
            VulkanImage image;
            vk::ImageView view;
            CreateImageAndImageView(image,
                                    view,
                                    m_ColorAttachmentSpecification[i].TextureFormat,
                                    m_ColorAttachmentSpecification[i].TextureUsage);
            if (m_ColorAttachmentSpecification[i].TextureUsage == FrameBufferTextureUsage::CPUAndGPU)
            {
                m_ColorAttachmentsReadBuffers[i].Buffer = m_GraphicsDevice.CreateBuffer(
                    m_Preferences.Width * m_Preferences.Height *
                        GetSizeOfPixel(m_ColorAttachmentSpecification[i].TextureFormat),
                    vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc,
                    VMA_MEMORY_USAGE_CPU_TO_GPU);
            }
            m_GraphicsDevice.TransitionImageLayout(
                image.Image,
                ConvertToVulkanFormat(m_ColorAttachmentSpecification[i].TextureFormat),
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::eShaderReadOnlyOptimal);
            vk::SamplerCreateInfo samplerCreateInfo{};
            samplerCreateInfo.magFilter =
                m_ColorAttachmentSpecification[i].TextureFormat == FrameBufferTextureFormat::RedInteger
                    ? vk::Filter::eNearest
                    : vk::Filter::eLinear;
            samplerCreateInfo.minFilter =
                m_ColorAttachmentSpecification[i].TextureFormat == FrameBufferTextureFormat::RedInteger
                    ? vk::Filter::eNearest
                    : vk::Filter::eLinear;
            samplerCreateInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
            samplerCreateInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
            samplerCreateInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
            samplerCreateInfo.anisotropyEnable = vk::True;
            samplerCreateInfo.maxAnisotropy = 16;
            samplerCreateInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
            samplerCreateInfo.unnormalizedCoordinates = vk::False;
            samplerCreateInfo.compareEnable = vk::False;
            samplerCreateInfo.compareOp = vk::CompareOp::eAlways;
            samplerCreateInfo.mipmapMode =
                m_ColorAttachmentSpecification[i].TextureFormat == FrameBufferTextureFormat::RedInteger
                    ? vk::SamplerMipmapMode::eNearest
                    : vk::SamplerMipmapMode::eLinear;
            samplerCreateInfo.mipLodBias = 0.0f;
            samplerCreateInfo.minLod = 0.0f;
            samplerCreateInfo.maxLod = 0.0f;

            vk::Sampler sampler = m_GraphicsDevice.GetDevice().createSampler(samplerCreateInfo);
            m_ColorAttachmentsTextures.emplace_back(m_Preferences.Width, m_Preferences.Height, image, view, sampler);
        }
        std::vector<BindingSetElement> elements;
        elements.reserve(m_ColorAttachmentsTextures.size());
        for (uint32_t i = 0; i < narrow_cast<uint32_t>(m_ColorAttachmentsTextures.size()); ++i)
        {
            elements.push_back({i, m_ColorAttachmentsTextures[i]});
        }
        m_ColorBindingSet = BindingSet::Create(BeeMove(elements));
        if (m_DepthAttachmentSpecification.TextureFormat != FrameBufferTextureFormat::None)
        {
            VulkanImage image;
            vk::ImageView view;
            CreateImageAndImageView(
                image, view, m_DepthAttachmentSpecification.TextureFormat, m_DepthAttachmentSpecification.TextureUsage);
            // m_GraphicsDevice.TransitionImageLayout(m_DepthAttachmentTexture.Image,
            // ConvertToVulkanFormat(m_DepthAttachmentSpecification.TextureFormat),
            //     vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthAttachmentOptimal);
            vk::SamplerCreateInfo samplerCreateInfo{};
            samplerCreateInfo.magFilter = vk::Filter::eLinear;
            samplerCreateInfo.minFilter = vk::Filter::eLinear;
            samplerCreateInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
            samplerCreateInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
            samplerCreateInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
            samplerCreateInfo.anisotropyEnable = vk::True;
            samplerCreateInfo.maxAnisotropy = 16;
            samplerCreateInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
            samplerCreateInfo.unnormalizedCoordinates = vk::False;
            samplerCreateInfo.compareEnable = vk::False;
            samplerCreateInfo.compareOp = vk::CompareOp::eAlways;
            samplerCreateInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
            samplerCreateInfo.mipLodBias = 0.0f;
            samplerCreateInfo.minLod = 0.0f;
            samplerCreateInfo.maxLod = 0.0f;
            vk::Sampler sampler = m_GraphicsDevice.GetDevice().createSampler(samplerCreateInfo);
            m_DepthAttachmentTexture =
                CreateScope<VulkanGPUTextureResource>(m_Preferences.Width, m_Preferences.Height, image, view, sampler);
        }
        m_Invalid = false;
    }

    uintptr_t VulkanFrameBuffer::GetColorAttachmentImGuiRendererID(uint32_t index) const
    {
        BeeExpects(index < m_ColorAttachmentsTextures.size());
        return m_ColorAttachmentsTextures[index].GetRendererID();
    }

    uintptr_t VulkanFrameBuffer::GetDepthAttachmentImGuiRendererID() const
    {
        BeeExpects(m_DepthAttachmentTexture);
        return m_DepthAttachmentTexture->GetRendererID();
    }

    int VulkanFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y) const
    {
        BeeExpects(attachmentIndex < m_ColorAttachmentsTextures.size());
        BeeExpects(x >= 0 && x < m_Preferences.Width && y >= 0 && y < m_Preferences.Height);
        BeeExpects(m_ColorAttachmentSpecification[attachmentIndex].TextureFormat ==
                   FrameBufferTextureFormat::RedInteger);
        BeeExpects(m_ColorAttachmentSpecification[attachmentIndex].TextureUsage == FrameBufferTextureUsage::CPUAndGPU);
        CopyToBufferIfNotCopied(attachmentIndex);
        void* data;
        vmaMapMemory(GetVulkanAllocator(), m_ColorAttachmentsReadBuffers[attachmentIndex].Buffer.Memory, &data);
        float32_t pixelFloat = ((float32_t*)data)[y * m_Preferences.Width + x];
        int pixel = (int)pixelFloat;
        vmaUnmapMemory(GetVulkanAllocator(), m_ColorAttachmentsReadBuffers[attachmentIndex].Buffer.Memory);
        return pixel;
    }
    VulkanImage VulkanFrameBuffer::GetColorAttachment(uint32_t index) const
    {
        BeeExpects(index < m_ColorAttachmentsTextures.size());
        return m_ColorAttachmentsTextures[index].GetVulkanImage();
    }

    DumpedImage VulkanFrameBuffer::DumpAttachment(uint32_t attachmentIndex) const
    {
        BeeExpects(attachmentIndex < m_ColorAttachmentsTextures.size());
        BeeExpects(m_ColorAttachmentSpecification[attachmentIndex].TextureUsage == FrameBufferTextureUsage::CPUAndGPU);
        CopyToBufferIfNotCopied(attachmentIndex);
        void* data;
        vmaMapMemory(GetVulkanAllocator(), m_ColorAttachmentsReadBuffers[attachmentIndex].Buffer.Memory, &data);
        DumpedImage image(data,
                          m_Preferences.Width,
                          m_Preferences.Height,
                          GetSizeOfPixel(m_ColorAttachmentSpecification[attachmentIndex].TextureFormat),
                          m_ColorAttachmentSpecification[attachmentIndex].TextureFormat);
        vmaUnmapMemory(GetVulkanAllocator(), m_ColorAttachmentsReadBuffers[attachmentIndex].Buffer.Memory);
        return image;
    }

    void VulkanFrameBuffer::CopyToBufferIfNotCopied(uint32_t index) const
    {
        BeeExpects(!m_Invalid && "FrameBuffer is in invalid state.");
        if (m_ColorAttachmentsReadBuffers[index].IsUpdated)
        {
            return;
        }
        m_ColorAttachmentsReadBuffers[index].IsUpdated = true;
        auto cmd = m_GraphicsDevice.BeginSingleTimeCommands();
        m_GraphicsDevice.TransitionImageLayout(
            cmd,
            m_ColorAttachmentsTextures[index].GetVulkanImage().Image,
            ConvertToVulkanFormat(m_ColorAttachmentSpecification[index].TextureFormat),
            vk::ImageLayout::eShaderReadOnlyOptimal,
            vk::ImageLayout::eTransferSrcOptimal);
        vk::BufferImageCopy region{};
        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.layerCount = 1;
        region.imageExtent.width = m_Preferences.Width;
        region.imageExtent.height = m_Preferences.Height;
        region.imageExtent.depth = 1;
        cmd.copyImageToBuffer(m_ColorAttachmentsTextures[index].GetVulkanImage().Image,
                              vk::ImageLayout::eTransferSrcOptimal,
                              m_ColorAttachmentsReadBuffers[index].Buffer.Buffer,
                              1,
                              &region);
        m_GraphicsDevice.TransitionImageLayout(
            cmd,
            m_ColorAttachmentsTextures[index].GetVulkanImage().Image,
            ConvertToVulkanFormat(m_ColorAttachmentSpecification[index].TextureFormat),
            vk::ImageLayout::eTransferSrcOptimal,
            vk::ImageLayout::eShaderReadOnlyOptimal);
        m_GraphicsDevice.EndSingleTimeCommands(cmd);
    }
} // namespace BeeEngine::Internal
