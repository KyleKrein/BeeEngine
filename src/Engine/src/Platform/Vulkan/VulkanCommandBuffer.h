//
// Created by alexl on 10.06.2023.
//

#pragma once
#if defined(BEE_COMPILE_VULKAN)
#include "vulkan/vulkan.hpp"
#include "VulkanRenderPass.h"

namespace BeeEngine::Internal
{
    class VulkanCommandBuffer
    {
    public:
        VulkanCommandBuffer() = default;
        VulkanCommandBuffer(vk::CommandBuffer& commandBuffer, vk::Framebuffer& framebuffer) : m_CommandBuffer(commandBuffer), m_Framebuffer(framebuffer) {}
        void RecordDrawCommands(VulkanRenderPass& renderPass, vk::Extent2D& swapChainExtent, vk::Pipeline& pipeline);
        vk::CommandBuffer& GetHandle()
        {
            return m_CommandBuffer;
        }
    private:
        vk::CommandBuffer m_CommandBuffer;
        vk::Framebuffer m_Framebuffer;
    };
}
#endif