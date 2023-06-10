//
// Created by alexl on 09.06.2023.
//

#pragma once

#include "Renderer/CommandPool.h"
#include "vulkan/vulkan.hpp"
#include "Renderer/QueueFamilyIndices.h"
#include "VulkanCommandBuffer.h"

namespace BeeEngine::Internal
{
    struct SwapChainFrame;
    class VulkanCommandPool: public CommandPool
    {
    public:
        VulkanCommandPool(vk::Device& device, const QueueFamilyIndices& queueFamilyIndices);
        ~VulkanCommandPool() override;
        vk::CommandPool& GetHandle()
        {
            return m_CommandPool;
        }
        void CreateCommandBuffers(std::vector<SwapChainFrame>& commandBuffers);
        VulkanCommandBuffer CreateCommandBuffer();
    private:
        vk::CommandPool m_CommandPool;
        vk::Device m_Device;
        vk::CommandBufferAllocateInfo m_AllocateInfo;
        std::vector<vk::CommandBuffer> m_CommandBuffers;
    };
}
