//
// Created by Aleksandr on 10.03.2024.
//

#pragma once
#include "Renderer/UniformBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanGraphicsDevice.h"

namespace BeeEngine::Internal
{

    class VulkanUniformBuffer final : public UniformBuffer
    {
    public:
        VulkanUniformBuffer(size_t size);

        std::vector<IBindable::BindGroupLayoutEntryType> GetBindGroupLayoutEntry() const override;

        std::vector<IBindable::BindGroupEntryType> GetBindGroupEntry() const override;

        ~VulkanUniformBuffer() override;

        void SetData(const void* data, size_t size) override;

    private:
        VulkanBuffer m_Buffer;
        size_t m_Size;
        VulkanGraphicsDevice& m_GraphicsDevice;
        vk::DescriptorBufferInfo m_DescriptorBufferInfo;
    };

} // namespace BeeEngine::Internal
