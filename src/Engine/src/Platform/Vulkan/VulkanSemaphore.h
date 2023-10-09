//
// Created by alexl on 10.06.2023.
//

#pragma once
#if defined(BEE_COMPILE_VULKAN)
#include "vulkan/vulkan.hpp"

namespace BeeEngine::Internal
{
    class VulkanSemaphore
    {
    public:
        VulkanSemaphore() = default;
        VulkanSemaphore(vk::Device& device);
        ~VulkanSemaphore();
        VulkanSemaphore(const VulkanSemaphore& other) = delete;
        VulkanSemaphore& operator=(const VulkanSemaphore& other ) = delete;
        VulkanSemaphore(VulkanSemaphore&& other) noexcept;
        VulkanSemaphore& operator=(VulkanSemaphore&& other) noexcept;
        vk::Semaphore& GetHandle()
        {
            return m_Semaphore;
        }
    private:
        vk::Semaphore m_Semaphore;
        vk::Device m_Device;
        bool m_IsInitialized = false;
    };
}
#endif