//
// Created by alexl on 10.06.2023.
//

#pragma once
#include "vulkan/vulkan.hpp"

namespace BeeEngine::Internal
{
    class VulkanCommandBuffer
    {
    public:
        VulkanCommandBuffer() = default;
        VulkanCommandBuffer(vk::CommandBuffer& commandBuffer) : m_CommandBuffer(commandBuffer) {}
        vk::CommandBuffer& GetHandle()
        {
            return m_CommandBuffer;
        }
    private:
        vk::CommandBuffer m_CommandBuffer;
    };
}
