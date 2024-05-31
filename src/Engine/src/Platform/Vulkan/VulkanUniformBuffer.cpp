//
// Created by Aleksandr on 10.03.2024.
//

#include "VulkanUniformBuffer.h"

namespace BeeEngine::Internal
{
    VulkanUniformBuffer::VulkanUniformBuffer(size_t size)
        : m_Size(size), m_GraphicsDevice(VulkanGraphicsDevice::GetInstance())
    {
        m_Buffer =
            m_GraphicsDevice.CreateBuffer(size, vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU);
        m_DescriptorBufferInfo.buffer = m_Buffer.Buffer;
        m_DescriptorBufferInfo.offset = 0;
        m_DescriptorBufferInfo.range = size;
    }

    void VulkanUniformBuffer::Bind(uint32_t slot) {}

    std::vector<IBindable::BindGroupLayoutEntryType> VulkanUniformBuffer::GetBindGroupLayoutEntry() const
    {
        vk::DescriptorSetLayoutBinding layoutBinding(0,
                                                     vk::DescriptorType::eUniformBuffer,
                                                     1,
                                                     vk::ShaderStageFlagBits::eVertex |
                                                         vk::ShaderStageFlagBits::eFragment);
        return {layoutBinding};
    }

    std::vector<IBindable::BindGroupEntryType> VulkanUniformBuffer::GetBindGroupEntry() const
    {
        vk::WriteDescriptorSet bufferWrite = {};
        bufferWrite.dstSet = nullptr;
        bufferWrite.dstBinding = 0;
        bufferWrite.dstArrayElement = 0;
        bufferWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
        bufferWrite.descriptorCount = 1;
        bufferWrite.pBufferInfo = &m_DescriptorBufferInfo;
        return {bufferWrite};
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
} // namespace BeeEngine::Internal
