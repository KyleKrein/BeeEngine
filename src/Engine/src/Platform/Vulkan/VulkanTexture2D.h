//
// Created by Aleksandr on 23.02.2024.
//

#pragma once
#include "VulkanGraphicsDevice.h"
#include "Renderer/Texture.h"

namespace BeeEngine::Internal
{
    class VulkanTexture2D: public Texture2D
    {
    public:
        void Bind(uint32_t slot=0) override{}

        std::vector<IBindable::BindGroupLayoutEntryType> GetBindGroupLayoutEntry() const override;

        std::vector<IBindable::BindGroupEntryType> GetBindGroupEntry() const override;

        void SetData(gsl::span<std::byte> data, uint32_t numberOfChannels) override;

        VulkanTexture2D(uint32_t width, uint32_t height, gsl::span<std::byte> data, uint32_t numberOfChannels, Filter filter, Filter mipmapFilter);
        ~VulkanTexture2D() override;
    private:
        void FreeResources();
        bool ShouldFreeResources();
        void CopyBufferToImageWithTransition(VulkanBuffer& buffer);

        VulkanGraphicsDevice& m_Device;
        VulkanImage m_Image {};
        vk::ImageView m_ImageView = nullptr;
        vk::Sampler m_Sampler = nullptr;
        vk::DescriptorImageInfo m_ImageInfo = {};
        Filter m_Filter;
        Filter m_MipmapFilter;
    };
}
