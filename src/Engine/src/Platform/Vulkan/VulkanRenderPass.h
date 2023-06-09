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
        VulkanRenderPass(vk::Format swapChainImageFormat);
        ~VulkanRenderPass();
        vk::RenderPass& GetHandle()
        {
            return m_RenderPass;
        }
    private:
        vk::RenderPass m_RenderPass;
        vk::Device m_Device;
    };
}
