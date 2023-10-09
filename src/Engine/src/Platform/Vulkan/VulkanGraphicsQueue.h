//
// Created by alexl on 09.06.2023.
//

#pragma once

#include "Renderer/GraphicsQueue.h"
#if defined(BEE_COMPILE_VULKAN)
#include "vulkan/vulkan.hpp"

namespace BeeEngine::Internal
{
    class VulkanGraphicsQueue: public GraphicsQueue
    {
    public:
        VulkanGraphicsQueue(uint32_t index, float* queuePriority);
        ~VulkanGraphicsQueue() override;
        void Initialize(vk::PhysicalDevice& physicalDevice, vk::Device& device);
        vk::DeviceQueueCreateInfo& GetQueueCreateInfo() { return m_QueueCreateInfo; }
        const vk::Queue& GetQueue() const { return m_Queue; }
    private:
        uint32_t m_Index;
        vk::DeviceQueueCreateInfo m_QueueCreateInfo;
        vk::Queue m_Queue;
    };
}
#endif