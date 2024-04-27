//
// Created by Aleksandr on 09.03.2024.
//

#include "VulkanInstancedBuffer.h"

#include "Renderer/CommandBuffer.h"

namespace BeeEngine::Internal {
    VulkanInstancedBuffer::VulkanInstancedBuffer(size_t size)
        : m_GraphicsDevice(VulkanGraphicsDevice::GetInstance()), m_Size(size)
    {
        m_Buffer = m_GraphicsDevice.CreateBuffer(m_Size, vk::BufferUsageFlagBits::eVertexBuffer, VMA_MEMORY_USAGE_AUTO);
    }

    VulkanInstancedBuffer::~VulkanInstancedBuffer()
    {
        m_GraphicsDevice.DestroyBuffer(m_Buffer);
    }

    void VulkanInstancedBuffer::SetData(void* data, size_t size)
    {
        BeeExpects(size <= m_Size);
        m_GraphicsDevice.CopyToBuffer({(byte*)data, size}, m_Buffer);
    }

    void VulkanInstancedBuffer::Bind(CommandBuffer& cmd)
    {
        vk::CommandBuffer commandBuffer = cmd.GetBufferHandleAs<vk::CommandBuffer>();
        vk::DeviceSize offsets[] = {{0}};
        commandBuffer.bindVertexBuffers(1, 1, &m_Buffer.Buffer, offsets);
    }

    size_t VulkanInstancedBuffer::GetSize()
    {
        return m_Size;
    }
}
