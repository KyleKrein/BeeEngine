//
// Created by Aleksandr on 10.03.2024.
//

#include "VulkanUniformBuffer.h"

namespace BeeEngine::Internal
{
    VulkanUniformBuffer::VulkanUniformBuffer(size_t size)
        : m_Size(size), m_GraphicsDevice(VulkanGraphicsDevice::GetInstance())
    {
        m_Buffer = m_GraphicsDevice.CreateBuffer(size, vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU);
    }

    void VulkanUniformBuffer::Bind(uint32_t slot)
    {
    }

    std::vector<IBindable::BindGroupLayoutEntryType> VulkanUniformBuffer::GetBindGroupLayoutEntry() const
    {
        return {};
    }

    std::vector<IBindable::BindGroupEntryType> VulkanUniformBuffer::GetBindGroupEntry() const
    {
        return {};
    }

    VulkanUniformBuffer::~VulkanUniformBuffer()
    {
        m_GraphicsDevice.DestroyBuffer(m_Buffer);
    }

    void VulkanUniformBuffer::SetData(void* data, size_t size)
    {
        BeeExpects(size == m_Size && data != nullptr);
        m_GraphicsDevice.CopyToBuffer({(byte*)data, size}, m_Buffer);
    }
}
