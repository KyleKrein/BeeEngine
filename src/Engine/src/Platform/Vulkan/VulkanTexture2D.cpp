//
// Created by Aleksandr on 23.02.2024.
//

#include "VulkanTexture2D.h"

#include "backends/imgui_impl_vulkan.h"

namespace BeeEngine::Internal
{
    void VulkanTexture2D::CopyBufferToImageWithTransition(VulkanBuffer& buffer)
    {
        m_Device.TransitionImageLayout(m_Image.Image,
                                       vk::Format::eR8G8B8A8Unorm,
                                       vk::ImageLayout::eUndefined,
                                       vk::ImageLayout::eTransferDstOptimal);
        m_Device.CopyBufferToImage(buffer.Buffer, m_Image.Image, m_Width, m_Height, 1);
        m_Device.TransitionImageLayout(m_Image.Image,
                                       vk::Format::eR8G8B8A8Unorm,
                                       vk::ImageLayout::eTransferDstOptimal,
                                       vk::ImageLayout::eShaderReadOnlyOptimal);
    }

    std::vector<IBindable::BindGroupLayoutEntryType> VulkanTexture2D::GetBindGroupLayoutEntry() const
    {
        vk::DescriptorSetLayoutBinding uboLayoutBinding;
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = vk::DescriptorType::eSampledImage;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex;
        uboLayoutBinding.pImmutableSamplers = nullptr;

        vk::DescriptorSetLayoutBinding samplerLayoutBinding;
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = vk::DescriptorType::eSampler;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex;

        return {uboLayoutBinding, samplerLayoutBinding};
    }

    std::vector<IBindable::BindGroupEntryType> VulkanTexture2D::GetBindGroupEntry() const
    {
        vk::WriteDescriptorSet imageWrite;
        imageWrite.dstBinding = 0;
        imageWrite.dstArrayElement = 0;
        imageWrite.descriptorType = vk::DescriptorType::eSampledImage;
        imageWrite.descriptorCount = 1;
        imageWrite.pImageInfo = &m_ImageInfo;

        vk::WriteDescriptorSet samplerWrite;
        samplerWrite.dstBinding = 1;
        samplerWrite.dstArrayElement = 0;
        samplerWrite.descriptorType = vk::DescriptorType::eSampler;
        samplerWrite.descriptorCount = 1;
        samplerWrite.pImageInfo = &m_ImageInfo;

        return {imageWrite, samplerWrite};
    }

    void VulkanTexture2D::SetData(gsl::span<std::byte> data, uint32_t numberOfChannels)
    {
        std::vector<std::byte> dataWithAlpha;
        if (numberOfChannels == 3)
        {
            dataWithAlpha.resize(data.size() * 4 / 3);
            for (int i = 0; i < data.size() / 3; i++)
            {
                dataWithAlpha[i * 4] = data[i * 3];
                dataWithAlpha[i * 4 + 1] = data[i * 3 + 1];
                dataWithAlpha[i * 4 + 2] = data[i * 3 + 2];
                dataWithAlpha[i * 4 + 3] = std::byte(255);
            }
        }
        else
        {
            dataWithAlpha.resize(data.size());
            std::memcpy(dataWithAlpha.data(), data.data(), data.size());
        }
        size_t imageSize = m_Width * m_Height * 4;
        VulkanBuffer buffer =
            m_Device.CreateBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_TO_GPU);
        void* mappedData = nullptr;
        if (vmaMapMemory(GetVulkanAllocator(), buffer.Memory, &mappedData) != VK_SUCCESS)
        {
            BeeCoreError("Failed to map memory");
        }
        std::memcpy(mappedData, dataWithAlpha.data(), imageSize);
        vmaUnmapMemory(GetVulkanAllocator(), buffer.Memory);
        if (!ShouldFreeResources())
        {
            vk::ImageCreateInfo imageCreateInfo;
            imageCreateInfo.sType = vk::StructureType::eImageCreateInfo;
            imageCreateInfo.imageType = vk::ImageType::e2D;
            imageCreateInfo.extent.width = m_Width;
            imageCreateInfo.extent.height = m_Height;
            imageCreateInfo.extent.depth = 1;
            imageCreateInfo.mipLevels = 1;
            imageCreateInfo.arrayLayers = 1;
            imageCreateInfo.format = vk::Format::eR8G8B8A8Unorm;
            imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
            imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
            imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
            imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
            imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
            imageCreateInfo.flags = {};
            vk::ImageViewCreateInfo imageViewCreateInfo;
            imageViewCreateInfo.sType = vk::StructureType::eImageViewCreateInfo;
            imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
            imageViewCreateInfo.format = vk::Format::eR8G8B8A8Unorm;
            imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            imageViewCreateInfo.subresourceRange.levelCount = 1;
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            imageViewCreateInfo.subresourceRange.layerCount = 1;
            m_Device.CreateImageWithInfo(imageCreateInfo,
                                         imageViewCreateInfo,
                                         vk::MemoryPropertyFlagBits::eDeviceLocal,
                                         VMA_MEMORY_USAGE_GPU_ONLY,
                                         m_Image,
                                         m_ImageView);

            CopyBufferToImageWithTransition(buffer);

            vk::SamplerCreateInfo samplerCreateInfo;
            samplerCreateInfo.sType = vk::StructureType::eSamplerCreateInfo;
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
            m_Sampler = m_Device.GetDevice().createSampler(samplerCreateInfo);
            m_RendererID = (uintptr_t)ImGui_ImplVulkan_AddTexture(
                m_Sampler, m_ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
        else
        {
            CopyBufferToImageWithTransition(buffer);
        }
        // buffer.Destroy();
        m_Device.DestroyBuffer(buffer);

        m_ImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        m_ImageInfo.imageView = m_ImageView;
        m_ImageInfo.sampler = m_Sampler;
        m_BindingSet = BindingSet::Create({{0, *this}});
    }

    VulkanTexture2D::VulkanTexture2D(uint32_t width,
                                     uint32_t height,
                                     gsl::span<std::byte> data,
                                     uint32_t numberOfChannels)
        : m_Device(VulkanGraphicsDevice::GetInstance())
    {
        m_Height = height;
        m_Width = width;

        VulkanTexture2D::SetData(data, numberOfChannels);
    }

    VulkanTexture2D::~VulkanTexture2D()
    {
        if (ShouldFreeResources())
            FreeResources();
    }

    void VulkanTexture2D::FreeResources()
    {
        auto device = m_Device.GetDevice();
        DeletionQueue::Frame().PushFunction([device, sampler = m_Sampler]() { device.destroySampler(sampler); });
        m_Device.DestroyImageWithView(m_Image, m_ImageView);
        ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet)m_RendererID);
        m_Sampler = nullptr;
        m_ImageView = nullptr;
        m_Image = {};
    }

    bool VulkanTexture2D::ShouldFreeResources()
    {
        return m_Sampler || m_ImageView || m_Image.Image || m_Image.Memory;
    }
} // namespace BeeEngine::Internal
