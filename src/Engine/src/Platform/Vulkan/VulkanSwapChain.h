//
// Created by alexl on 09.06.2023.
//

#pragma once
#include "vulkan/vulkan.hpp"
#include "Renderer/QueueFamilyIndices.h"
#include "Renderer/SwapChain.h"

namespace BeeEngine::Internal
{
    class VulkanGraphicsDevice;
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
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
        VulkanSwapChain(VulkanGraphicsDevice& graphicsDevice, uint32_t width, uint32_t height);
        ~VulkanSwapChain() override;
        VulkanSwapChain(const VulkanSwapChain& other) = delete;
        VulkanSwapChain& operator=(const VulkanSwapChain& other ) = delete;
        void Recreate(uint32_t width, uint32_t height);

        VkSwapchainKHR& GetHandle()
        {
            return m_SwapChain;
        }
        VkFormat& GetFormat()
        {
            return m_SurfaceFormat.format;
        }
        VkExtent2D& GetExtent()
        {
            return m_Extent;
        }
        VkSurfaceFormatKHR& GetSurfaceFormat()
        {
            return m_SurfaceFormat;
        }
        VkPresentModeKHR& GetPresentMode()
        {
            return m_PresentMode;
        }

        vk::RenderPass GetRenderPass();
        size_t ImageCount();
        vk::Framebuffer GetFrameBuffer(size_t index)
        {
            return m_SwapChainFramebuffers[index];
        }

        VkResult AcquireNextImage(uint32_t *imageIndex);
        VkResult SubmitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

        bool ResizeInProgress();

    private:
        SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface);

        void ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
        void ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes);

        void ChooseExtent(VkSurfaceCapabilitiesKHR &capabilities);

        void CreateCommandBuffers();
        void CreateSwapChain();
        void CreateImageViews();
        void CreateDepthResources();
        void CreateRenderPass();
        void CreateFramebuffers();
        void CreateSyncObjects();
        void Destroy();

    private:
        VkSurfaceFormatKHR m_SurfaceFormat;
        VkPresentModeKHR m_PresentMode;
        VkExtent2D m_Extent;

        VkRenderPass m_RenderPass;
        VkSwapchainKHR m_SwapChain;
        //std::vector<SwapChainFrame> m_Frames;
        uint32_t m_MaxFrames;
        uint32_t m_CurrentFrame = 0;
        QueueFamilyIndices m_QueueFamilyIndices;
        SwapChainSupportDetails m_SwapChainSupportDetails;
        VulkanGraphicsDevice& m_GraphicsDevice;

        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_InFlightFences;
        std::vector<VkFence> m_ImagesInFlight;

        std::vector<VkImage> m_DepthImages;
        std::vector<VkDeviceMemory> m_DepthImageMemorys;
        std::vector<VkImageView> m_DepthImageViews;
        std::vector<VkImage> m_SwapChainImages;
        std::vector<VkImageView> m_SwapChainImageViews;
        std::vector<VkFramebuffer> m_SwapChainFramebuffers;

        bool m_ResizeInProgress = false;

        VkFormat FindDepthFormat();
    };
}
