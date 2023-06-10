//
// Created by alexl on 09.06.2023.
//

#pragma once
#include "vulkan/vulkan.hpp"
#include "Renderer/QueueFamilyIndices.h"
#include "Renderer/SwapChain.h"
#include "VulkanFramebuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSemaphore.h"
#include "VulkanFence.h"

namespace BeeEngine::Internal
{
    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };
    struct SwapChainFrame
    {
        vk::Image Image;
        vk::ImageView ImageView;
        VulkanFramebuffer Framebuffer;
        VulkanCommandBuffer CommandBuffer;
        VulkanSemaphore ImageAvailableSemaphore;
        VulkanSemaphore RenderFinishedSemaphore;
        VulkanFence InFlightFence;
    };
    class VulkanSwapChain: public SwapChain
    {
    public:
        VulkanSwapChain(vk::PhysicalDevice &physicalDevice, vk::Device& logicalDevice, vk::SurfaceKHR &surface, uint32_t width, uint32_t height, QueueFamilyIndices &queueFamilyIndices);
        ~VulkanSwapChain() override;
        VulkanSwapChain(const VulkanSwapChain& other) = delete;
        VulkanSwapChain& operator=(const VulkanSwapChain& other ) = delete;
        void Recreate();

        vk::SwapchainKHR& GetHandle()
        {
            return m_SwapChain;
        }
        vk::Format& GetFormat()
        {
            return m_SurfaceFormat.format;
        }
        vk::Extent2D& GetExtent()
        {
            return m_Extent;
        }
        vk::SurfaceFormatKHR& GetSurfaceFormat()
        {
            return m_SurfaceFormat;
        }
        vk::PresentModeKHR& GetPresentMode()
        {
            return m_PresentMode;
        }
        std::vector<SwapChainFrame>& GetFrames()
        {
            return m_Frames;
        }
    private:
        SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface);

        void ChooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats);
        void Destroy();
        void Create(uint32_t width, uint32_t height);

    private:
        vk::SurfaceFormatKHR m_SurfaceFormat;
        vk::PresentModeKHR m_PresentMode;
        vk::Extent2D m_Extent;

        vk::SwapchainKHR m_SwapChain;
        std::vector<SwapChainFrame> m_Frames;
        //vk::Format m_Format;
        uint32_t m_MaxFrames;
        vk::SurfaceKHR m_Surface;
        QueueFamilyIndices m_QueueFamilyIndices;
        SwapChainSupportDetails m_SwapChainSupportDetails;

        vk::Device m_LogicalDevice;
        vk::PhysicalDevice m_PhysicalDevice;

        void ChoosePresentMode(const std::vector<vk::PresentModeKHR>& presentModes);

        void ChooseExtent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR &capabilities);

        void CreateImageViews();

        void CreateSyncronizationObjects();

        void CreateFramebuffers();

        void CreateCommandBuffers();
    };
}
