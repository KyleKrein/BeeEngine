//
// Created by alexl on 10.06.2023.
//
#if defined(BEE_COMPILE_VULKAN)
#include "VulkanSemaphore.h"


namespace BeeEngine::Internal
{

    VulkanSemaphore::VulkanSemaphore(vk::Device &device)
    : m_Device(device), m_IsInitialized(true)
    {
        vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.flags = vk::SemaphoreCreateFlags();
        m_Semaphore = m_Device.createSemaphore(semaphoreCreateInfo);
    }

    VulkanSemaphore::~VulkanSemaphore()
    {
        if(m_IsInitialized)
        {
            m_Device.waitIdle();
            m_Device.destroySemaphore(m_Semaphore);
        }
    }

    VulkanSemaphore::VulkanSemaphore(VulkanSemaphore &&other) noexcept
    {
        m_Device = other.m_Device;
        m_Semaphore = other.m_Semaphore;
        m_IsInitialized = other.m_IsInitialized;

        other.m_IsInitialized = false;
    }

    VulkanSemaphore& VulkanSemaphore::operator=(VulkanSemaphore &&other) noexcept
    {
        m_Device = other.m_Device;
        m_Semaphore = other.m_Semaphore;
        m_IsInitialized = other.m_IsInitialized;

        other.m_IsInitialized = false;

        return *this;
    }
}
#endif