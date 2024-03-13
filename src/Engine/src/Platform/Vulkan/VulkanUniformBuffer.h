//
// Created by Aleksandr on 10.03.2024.
//

#pragma once
#include "VulkanBuffer.h"
#include "VulkanGraphicsDevice.h"
#include "Renderer/UniformBuffer.h"

namespace BeeEngine::Internal
{

    class VulkanUniformBuffer final: public UniformBuffer
    {
    public:
        VulkanUniformBuffer(size_t size);
        void Bind(uint32_t slot) override;

        std::vector<IBindable::BindGroupLayoutEntryType> GetBindGroupLayoutEntry() const override;

        std::vector<IBindable::BindGroupEntryType> GetBindGroupEntry() const override;

        ~VulkanUniformBuffer() override;

        void SetData(void* data, size_t size) override;
    private:
        VulkanBuffer m_Buffer;
        size_t m_Size;
        VulkanGraphicsDevice& m_GraphicsDevice;
    };

}
