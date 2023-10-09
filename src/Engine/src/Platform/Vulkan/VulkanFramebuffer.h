//
// Created by alexl on 10.06.2023.
//

#pragma once
#if defined(BEE_COMPILE_VULKAN)
#include "vulkan/vulkan.hpp"
namespace BeeEngine::Internal
{
    struct SwapChainFrame;
    class VulkanFramebuffer
    {
    public:
        VulkanFramebuffer() = default;
        VulkanFramebuffer(vk::Device& device, vk::RenderPass& renderPass, vk::Extent2D& swapChainExtent, vk::ImageView& imageView);
        VulkanFramebuffer(vk::Device& device, const vk::Framebuffer& framebuffer) : m_Framebuffer(framebuffer), m_Device(device), m_IsInitialized(true) {}
        ~VulkanFramebuffer();
        VulkanFramebuffer(const VulkanFramebuffer& other) = delete;
        VulkanFramebuffer& operator=(const VulkanFramebuffer& other ) = delete;
        VulkanFramebuffer(VulkanFramebuffer&& other) noexcept;
        VulkanFramebuffer& operator=(VulkanFramebuffer&& other) noexcept;
        static void CreateFramebuffers(vk::Device& device, vk::RenderPass& renderPass, vk::Extent2D& swapChainExtent, std::vector<SwapChainFrame>& framesToGetFramebuffers);
        vk::Framebuffer& GetHandle()
        {
            return m_Framebuffer;
        }
    private:
        bool m_IsInitialized = false;
        vk::Framebuffer m_Framebuffer;
        vk::Device m_Device;
    };
}
#endif