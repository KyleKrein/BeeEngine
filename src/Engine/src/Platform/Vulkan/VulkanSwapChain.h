//
// Created by alexl on 09.06.2023.
//

#pragma once
#include "vulkan/vulkan.hpp"
#include "Renderer/QueueFamilyIndices.h"
#include "Renderer/SwapChain.h"

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
    };
    class VulkanSwapChain: public SwapChain
    {
    public:
        VulkanSwapChain(vk::PhysicalDevice &physicalDevice, vk::Device& logicalDevice, vk::SurfaceKHR &surface, uint32_t width, uint32_t height, QueueFamilyIndices &queueFamilyIndices);
        ~VulkanSwapChain() override;

        vk::SwapchainKHR& GetHandle()
        {
            return m_SwapChain;
        }
        vk::Format& GetFormat()
        {
            return m_Format;
        }
        vk::Extent2D& GetExtent()
        {
            return m_Extent;
        }
    private:
        SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface);

        void ChooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats);

    private:
        vk::SurfaceFormatKHR m_SurfaceFormat;
        vk::PresentModeKHR m_PresentMode;
        vk::Extent2D m_Extent;

        vk::SwapchainKHR m_SwapChain;
        std::vector<SwapChainFrame> m_Frames;
        vk::Format m_Format;

        vk::Device& m_LogicalDevice;

        void ChoosePresentMode(const std::vector<vk::PresentModeKHR>& presentModes);

        void ChooseExtent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR &capabilities);
    };
}
