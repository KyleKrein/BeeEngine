//
// Created by alexl on 09.06.2023.
//

#pragma once
#include "VulkanImage.h"
#if defined(BEE_COMPILE_VULKAN)
#include "vulkan/vulkan.hpp"
#include "Renderer/QueueFamilyIndices.h"
#include "Renderer/SwapChain.h"

namespace BeeEngine::Internal
{
    class VulkanGraphicsDevice;
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
        vk::Image DepthImage;
        vk::ImageView DepthImageView;
        vk::DeviceMemory DepthImageMemory;
        vk::Framebuffer Framebuffer;
        vk::CommandBuffer CommandBuffer;
        vk::Semaphore ImageAvailableSemaphore;
        vk::Semaphore RenderFinishedSemaphore;
        vk::Fence InFlightFence;
    };
    class VulkanSwapChain: public SwapChain
    {
    public:
        VulkanSwapChain(VulkanGraphicsDevice& graphicsDevice, uint32_t width, uint32_t height, VkSwapchainKHR oldSwapChain = VK_NULL_HANDLE);
        ~VulkanSwapChain() override;
        VulkanSwapChain(const VulkanSwapChain& other) = delete;
        VulkanSwapChain& operator=(const VulkanSwapChain& other ) = delete;

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

        vk::RenderPass GetRenderPass();
        size_t ImageCount();
        vk::Framebuffer GetFrameBuffer(size_t index)
        {
            return m_SwapChainFramebuffers[index];
        }

        vk::Result AcquireNextImage(uint32_t *imageIndex);
        vk::Result SubmitCommandBuffers(const vk::CommandBuffer *buffers, uint32_t *imageIndex);

        bool ResizeInProgress();

    private:
        SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface);

        void ChooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats);
        void ChoosePresentMode(const std::vector<vk::PresentModeKHR>& presentModes);

        void ChooseExtent(vk::SurfaceCapabilitiesKHR &capabilities);

        void CreateCommandBuffers();
        void CreateSwapChain();
        void CreateImageViews();
        void CreateDepthResources();
        void CreateRenderPass();
        void CreateFramebuffers();
        void CreateSyncObjects();
        void Destroy();

    private:
        vk::SurfaceFormatKHR m_SurfaceFormat;
        vk::PresentModeKHR m_PresentMode;
        vk::Extent2D m_Extent;

        vk::RenderPass m_RenderPass;
        vk::SwapchainKHR m_SwapChain;
        //std::vector<SwapChainFrame> m_Frames;
        uint32_t m_MaxFrames;
        uint32_t m_CurrentFrame = 0;
        QueueFamilyIndices m_QueueFamilyIndices;
        SwapChainSupportDetails m_SwapChainSupportDetails;
        VulkanGraphicsDevice& m_GraphicsDevice;

        std::vector<vk::Semaphore> m_ImageAvailableSemaphores;
        std::vector<vk::Semaphore> m_RenderFinishedSemaphores;
        std::vector<vk::Fence> m_InFlightFences;
        std::vector<vk::Fence> m_ImagesInFlight;

        std::vector<VulkanImage> m_DepthImages;
        std::vector<vk::ImageView> m_DepthImageViews;
        std::vector<vk::Image> m_SwapChainImages;
        std::vector<vk::ImageView> m_SwapChainImageViews;
        std::vector<vk::Framebuffer> m_SwapChainFramebuffers;

        bool m_ResizeInProgress = false;

        vk::Format FindDepthFormat();
    };
}
#endif