//
// Created by alexl on 09.06.2023.
//
#if defined(BEE_COMPILE_VULKAN)
#include "VulkanGraphicsQueue.h"


namespace BeeEngine::Internal
{

    VulkanGraphicsQueue::~VulkanGraphicsQueue()
    {

    }

    VulkanGraphicsQueue::VulkanGraphicsQueue(uint32_t index, float *queuePriority)
    {
        m_QueueCreateInfo = {
                vk::DeviceQueueCreateFlags(),
                index,
                1,
                queuePriority
        };
        m_Index = index;
    }

    void VulkanGraphicsQueue::Initialize(vk::PhysicalDevice &physicalDevice, vk::Device &device)
    {
        m_Queue = device.getQueue(m_Index, 0);
    }
}
#endif