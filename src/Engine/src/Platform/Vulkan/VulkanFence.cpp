//
// Created by alexl on 10.06.2023.
//

#include "VulkanFence.h"


namespace BeeEngine::Internal
{

    VulkanFence::VulkanFence(vk::Device &device)
    : m_Device(device), m_IsInitialized(true)
    {
        vk::FenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;
        m_Fence = device.createFence(fenceCreateInfo);
    }

    VulkanFence::~VulkanFence()
    {
        if(m_IsInitialized)
            m_Device.destroyFence(m_Fence);
    }

    VulkanFence::VulkanFence(VulkanFence &&other)
    {
        m_Device = other.m_Device;
        m_Fence = other.m_Fence;
        m_IsInitialized = other.m_IsInitialized;

        other.m_IsInitialized = false;
    }

    VulkanFence& VulkanFence::operator=(VulkanFence &&other) noexcept
    {
        m_Device = other.m_Device;
        m_Fence = other.m_Fence;
        m_IsInitialized = other.m_IsInitialized;

        other.m_IsInitialized = false;

        return *this;
    }
}
