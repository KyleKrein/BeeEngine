//
// Created by Aleksandr on 10.03.2024.
//

#include "VulkanFrameBuffer.h"

#include "Utils.h"
#include "backends/imgui_impl_vulkan.h"
#include "Renderer/Renderer.h"

namespace BeeEngine::Internal
{
    vk::Format ConvertToVulkanFormat(FrameBufferTextureFormat format)
    {
        switch (format)
        {
            case FrameBufferTextureFormat::RGBA8:
                return vk::Format::eR8G8B8A8Unorm;
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
    void VulkanFrameBuffer::CreateImageAndImageView(VulkanImage& image, vk::ImageView& view, FrameBufferTextureFormat format, FrameBufferTextureUsage usage)
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
        imageCreateInfo.usage = IsDepthFormat(format) ? vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled : vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled;
        if(usage == FrameBufferTextureUsage::CPUAndGPU)
        {
            //imageCreateInfo.usage |= vk::ImageUsageFlagBits::eTransferSrc;
        }
        imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
        imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;

        vk::ImageViewCreateInfo viewCreateInfo{};
        viewCreateInfo.viewType = vk::ImageViewType::e2D;
        viewCreateInfo.format = imageCreateInfo.format;
        viewCreateInfo.subresourceRange.aspectMask = IsDepthFormat(format) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;

        vk::MemoryPropertyFlags memoryPropertyFlags = usage == FrameBufferTextureUsage::GPUOnly ? vk::MemoryPropertyFlagBits::eDeviceLocal : vk::MemoryPropertyFlagBits::eDeviceLocal;

        VmaMemoryUsage memoryUsage = usage == FrameBufferTextureUsage::GPUOnly ? VMA_MEMORY_USAGE_GPU_ONLY : VMA_MEMORY_USAGE_GPU_ONLY;
        m_GraphicsDevice.CreateImageWithInfo(imageCreateInfo, viewCreateInfo, memoryPropertyFlags, memoryUsage, image, view);
    }
    VulkanFrameBuffer::VulkanFrameBuffer(const FrameBufferPreferences& preferences)
    : m_Preferences(preferences), m_GraphicsDevice(VulkanGraphicsDevice::GetInstance())
    {
        BEE_PROFILE_FUNCTION();
        for (auto specification: m_Preferences.Attachments.Attachments)
        {
            if(!IsDepthFormat(specification.TextureFormat))
            {
                m_ColorAttachmentSpecification.emplace_back(specification);
            }
            else
            {
                m_DepthAttachmentSpecification = specification;
            }
        }
        //m_CurrentCommandBuffer = m_GraphicsDevice.CreateCommandBuffer();
        VulkanFrameBuffer::Invalidate();
        m_Initiated = true;
    }

    VulkanFrameBuffer::~VulkanFrameBuffer()
    {
        BeeExpects(m_CurrentCommandBuffer == nullptr);
        BeeExpects(m_ColorAttachmentsTextures.size() == m_ColorAttachmentsTextureViews.size() &&
            m_ColorAttachmentsTextures.size() == m_ColorDescriptorSets.size() &&
            m_ColorAttachmentsTextures.size() == m_ColorSamplers.size());
        DeletionQueue::Frame().PushFunction([colorTextures = std::move(m_ColorAttachmentsTextures), colorViews = std::move(m_ColorAttachmentsTextureViews), colorSets = std::move(m_ColorDescriptorSets), colorSamplers = std::move(m_ColorSamplers), depthTexture = m_DepthAttachmentTexture, depthView = m_DepthAttachmentTextureView, depthSampler = m_DepthSampler, depthSet = m_DepthDescriptorSet]()mutable
        {
            auto& device = VulkanGraphicsDevice::GetInstance();
            for (size_t i = 0; i < colorTextures.size(); ++i)
            {
                device.DestroyImageWithView(colorTextures[i], colorViews[i]);
                device.GetDevice().destroySampler(colorSamplers[i]);
                ImGui_ImplVulkan_RemoveTexture(colorSets[i]);
            }
            if(depthTexture)
            {
                device.DestroyImageWithView(depthTexture, depthView);
                device.GetDevice().destroySampler(depthSampler);
                ImGui_ImplVulkan_RemoveTexture(depthSet);
            }
        });
    }

    CommandBuffer VulkanFrameBuffer::Bind()
    {
        BEE_PROFILE_FUNCTION();
        if(m_Invalid)
        {
            Invalidate();
        }
        m_CurrentCommandBuffer = m_GraphicsDevice.BeginSingleTimeCommands();

        std::vector<vk::RenderingAttachmentInfo> colorAttachments{};
        const auto size = m_ColorAttachmentSpecification.size();

        colorAttachments.reserve(size);


        for(size_t i = 0; i < size; ++i)
        {
            vk::RenderingAttachmentInfo colorAttachment{};
            colorAttachment.imageView = m_ColorAttachmentsTextureViews[i];
            colorAttachment.resolveMode = vk::ResolveModeFlagBits::eNone;
            colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
            colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
            colorAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
            colorAttachment.clearValue = m_ColorAttachmentSpecification[i].ClearColor;
            colorAttachments.push_back(colorAttachment);
        }

        vk::RenderingAttachmentInfo depthStencilAttachment{};
        depthStencilAttachment.imageView = m_DepthAttachmentTextureView;
        depthStencilAttachment.clearValue.depthStencil = vk::ClearDepthStencilValue{m_DepthAttachmentSpecification.ClearDepth, 0};
        depthStencilAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        depthStencilAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        depthStencilAttachment.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::RenderingInfo renderInfo{};
        renderInfo.colorAttachmentCount = colorAttachments.size();
        renderInfo.pColorAttachments = colorAttachments.data();
        renderInfo.layerCount = 1;
        renderInfo.renderArea = vk::Rect2D{{0, 0}, {m_Preferences.Width, m_Preferences.Height}};
        if(m_DepthAttachmentTexture)
        {
            renderInfo.pDepthAttachment = &depthStencilAttachment;
        }
        else
        {
            renderInfo.pDepthAttachment = nullptr;
        }

        m_CurrentCommandBuffer.beginRendering(&renderInfo, g_vkDynamicLoader);

        // Установка вьюпорта и сциззора
        vk::Viewport viewport =  m_GraphicsDevice.CreateVKViewport(m_Preferences.Width, m_Preferences.Height, 0.0f, 1.0f);
        vk::Rect2D scissor = {
            {0, 0},
            {m_Preferences.Width, m_Preferences.Height}
        };

        m_CurrentCommandBuffer.setViewport(0, 1, &viewport);
        m_CurrentCommandBuffer.setScissor(0, 1, &scissor);

        CommandBuffer commandBuffer {m_CurrentCommandBuffer, &m_RenderingQueue};
        commandBuffer.BeginRecording();
        return commandBuffer;
    }

    void VulkanFrameBuffer::Unbind(CommandBuffer& commandBuffer)
    {
        BeeExpects(m_CurrentCommandBuffer == commandBuffer.GetBufferHandleAs<vk::CommandBuffer>() && m_CurrentCommandBuffer != nullptr);
        BEE_PROFILE_FUNCTION();
        commandBuffer.EndRecording();//Flush();
        m_CurrentCommandBuffer.endRendering(g_vkDynamicLoader);
        m_GraphicsDevice.EndSingleTimeCommands(m_CurrentCommandBuffer);
        //Renderer::SubmitCommandBuffer({m_CurrentCommandBuffer});
        commandBuffer.Invalidate();
        m_CurrentCommandBuffer = nullptr;
    }

    void VulkanFrameBuffer::Resize(uint32_t width, uint32_t height)
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(width > 0 && height > 0 && width < std::numeric_limits<uint32_t>::max() && height < std::numeric_limits<uint32_t>::max());
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
        if(m_Initiated)
        {
            for (size_t i = 0; i < m_ColorAttachmentsTextures.size(); ++i)
            {
                m_GraphicsDevice.DestroyImageWithView(m_ColorAttachmentsTextures[i], m_ColorAttachmentsTextureViews[i]);
                ImGui_ImplVulkan_RemoveTexture(m_ColorDescriptorSets[i]);
                m_GraphicsDevice.GetDevice().destroySampler(m_ColorSamplers[i]);
            }
            if(m_DepthAttachmentTexture)
            {
                m_GraphicsDevice.DestroyImageWithView(m_DepthAttachmentTexture, m_DepthAttachmentTextureView);
                //m_GraphicsDevice.DestroyDescriptorSet(m_DepthDescriptorSet);
                m_GraphicsDevice.GetDevice().destroySampler(m_DepthSampler);
                ImGui_ImplVulkan_RemoveTexture(m_DepthDescriptorSet);
            }
            m_ColorAttachmentsTextures.clear();
            m_ColorAttachmentsTextureViews.clear();
            m_DepthAttachmentTexture = {};
            m_DepthAttachmentTextureView = nullptr;

            m_ColorDescriptorSets.clear();
            m_DepthDescriptorSet = nullptr;

            m_ColorSamplers.clear();
            m_DepthSampler = nullptr;
        }
        m_ColorAttachmentsTextures.resize(m_ColorAttachmentSpecification.size());
        m_ColorAttachmentsTextureViews.resize(m_ColorAttachmentSpecification.size());
        m_ColorDescriptorSets.resize(m_ColorAttachmentSpecification.size());
        m_ColorSamplers.resize(m_ColorAttachmentSpecification.size());
        for (size_t i = 0; i < m_ColorAttachmentSpecification.size(); ++i)
        {
            CreateImageAndImageView(m_ColorAttachmentsTextures[i], m_ColorAttachmentsTextureViews[i],
                m_ColorAttachmentSpecification[i].TextureFormat, m_ColorAttachmentSpecification[i].TextureUsage);
            m_GraphicsDevice.TransitionImageLayout(m_ColorAttachmentsTextures[i].Image, ConvertToVulkanFormat(m_ColorAttachmentSpecification[i].TextureFormat),
                vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal);
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

            m_ColorSamplers[i] = m_GraphicsDevice.GetDevice().createSampler(samplerCreateInfo);
            m_ColorDescriptorSets[i] = ImGui_ImplVulkan_AddTexture(m_ColorSamplers[i], m_ColorAttachmentsTextureViews[i], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
        if(m_DepthAttachmentSpecification.TextureFormat != FrameBufferTextureFormat::None)
        {
            CreateImageAndImageView(m_DepthAttachmentTexture, m_DepthAttachmentTextureView,
                m_DepthAttachmentSpecification.TextureFormat, m_DepthAttachmentSpecification.TextureUsage);
            //m_GraphicsDevice.TransitionImageLayout(m_DepthAttachmentTexture.Image, ConvertToVulkanFormat(m_DepthAttachmentSpecification.TextureFormat),
            //    vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthAttachmentOptimal);
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
            m_DepthSampler = m_GraphicsDevice.GetDevice().createSampler(samplerCreateInfo);
            m_DepthDescriptorSet = ImGui_ImplVulkan_AddTexture(m_DepthSampler, m_DepthAttachmentTextureView, VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL);
        }
        m_Invalid = false;
    }

    uintptr_t VulkanFrameBuffer::GetColorAttachmentRendererID(uint32_t index) const
    {
        BeeExpects(index < m_ColorDescriptorSets.size());
        return (uintptr_t)(VkDescriptorSet)m_ColorDescriptorSets[index];
    }

    uintptr_t VulkanFrameBuffer::GetDepthAttachmentRendererID() const
    {
        return (uintptr_t)(VkDescriptorSet)m_DepthDescriptorSet;
    }

    int VulkanFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y) const
    {
        BeeExpects(attachmentIndex < m_ColorAttachmentsTextures.size());
        BeeExpects(x >= 0 && x < m_Preferences.Width && y >= 0 && y < m_Preferences.Height);
        BeeExpects(m_ColorAttachmentSpecification[attachmentIndex].TextureFormat == FrameBufferTextureFormat::RedInteger);
        BeeExpects(m_ColorAttachmentSpecification[attachmentIndex].TextureUsage == FrameBufferTextureUsage::CPUAndGPU);
        return 0;
        void* data;
        vmaMapMemory(GetVulkanAllocator(), m_ColorAttachmentsTextures[attachmentIndex].Memory, &data);
        int pixel = ((float*)data)[y * m_Preferences.Width + x];
        vmaUnmapMemory(GetVulkanAllocator(), m_ColorAttachmentsTextures[attachmentIndex].Memory);

        return pixel;
    }
}
