//
// Created by alexl on 10.06.2023.
//

#pragma once
#if defined(BEE_COMPILE_VULKAN)
#include "vulkan/vulkan.hpp"

namespace BeeEngine::Internal
{
    class VulkanFence
    {
    public:
        VulkanFence() = default;
        VulkanFence(vk::Device& device);
        VulkanFence(const VulkanFence& other) = delete;
        VulkanFence& operator=(const VulkanFence& other ) = delete;
        VulkanFence(VulkanFence&& other);
        VulkanFence& operator=(VulkanFence&& other) noexcept;
        ~VulkanFence();
        vk::Fence& GetHandle()
        {
            return m_Fence;
        }
    private:
        bool m_IsInitialized = false;
        vk::Fence m_Fence;
        vk::Device m_Device;
    };
}
#endif