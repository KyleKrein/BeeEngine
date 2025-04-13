//
// Created by Aleksandr on 23.02.2024.
//

#pragma once
#include <span>
#include "Renderer/Texture.h"
#include "VulkanGraphicsDevice.h"

namespace BeeEngine::Internal
{
    class VulkanGPUTextureResource : public BeeEngine::GPUTextureResource
    {
    public:
        std::vector<IBindable::BindGroupLayoutEntryType> GetBindGroupLayoutEntry() const override;

        std::vector<IBindable::BindGroupEntryType> GetBindGroupEntry() const override;

        void SetData(std::span<std::byte> data, uint32_t numberOfChannels) override;
        VulkanGPUTextureResource(
            uint32_t width, uint32_t height, VulkanImage image, vk::ImageView view, vk::Sampler sampler);
        VulkanImage& GetVulkanImage() { return m_Image; }
        VulkanImage GetVulkanImage() const { return m_Image; }
        vk::ImageView& GetVulkanImageView() { return m_ImageView; }

        VulkanGPUTextureResource(uint32_t width, uint32_t height, std::span<std::byte> data, uint32_t numberOfChannels);
        ~VulkanGPUTextureResource() override;

    private:
        void FreeResources();
        bool ShouldFreeResources();
        void CopyBufferToImageWithTransition(VulkanBuffer& buffer);

        VulkanGraphicsDevice& m_Device;
        VulkanImage m_Image{};
        vk::ImageView m_ImageView = nullptr;
        vk::Sampler m_Sampler = nullptr;
        vk::DescriptorImageInfo m_ImageInfo = {};
    };
} // namespace BeeEngine::Internal
