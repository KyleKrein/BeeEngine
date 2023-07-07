//
// Created by alexl on 09.06.2023.
//

#pragma once
#include "vulkan/vulkan.hpp"

namespace BeeEngine::Internal
{
    class VulkanRenderPass
    {
    public:
        VulkanRenderPass() = default;
        VulkanRenderPass(vk::Device& device, vk::Format swapChainImageFormat);
        ~VulkanRenderPass();
        VulkanRenderPass(const VulkanRenderPass& other) = delete;
        VulkanRenderPass& operator=(const VulkanRenderPass& other ) = delete;
        VulkanRenderPass(VulkanRenderPass&& other);
        VulkanRenderPass& operator=(VulkanRenderPass&& other);
        vk::RenderPass& GetHandle()
        {
            return m_RenderPass;
        }
    private:
        bool m_IsInitialized = false;
        vk::RenderPass m_RenderPass;
        vk::Device m_Device;
    };
}
