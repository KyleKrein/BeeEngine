//
// Created by alexl on 09.06.2023.
//

#include "VulkanSwapChain.h"
#include "Core/Logging/Log.h"
#include "shaderc/shaderc.hpp"
#include "Windowing/WindowHandler/WindowHandler.h"
#include "VulkanGraphicsDevice.h"


namespace BeeEngine::Internal
{

    VulkanSwapChain::VulkanSwapChain(VulkanGraphicsDevice& graphicsDevice, uint32_t width, uint32_t height)
    : m_GraphicsDevice(graphicsDevice), m_Extent(vk::Extent2D(width, height))
    {
        CreateSwapChain();
        CreateImageViews();
        CreateRenderPass();
        CreateDepthResources();
        CreateFramebuffers();
        //CreateCommandBuffers();
        CreateSyncObjects();
    }

    SwapChainSupportDetails VulkanSwapChain::QuerySwapChainSupport(vk::PhysicalDevice &physicalDevice, vk::SurfaceKHR &surface)
    {
        SwapChainSupportDetails details;
        details.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);

        BeeCoreInfo("SwapChain can support the following surface capabilities:");
        BeeCoreInfo("minImageCount: {}", details.capabilities.minImageCount);
        BeeCoreInfo("maxImageCount: {}", details.capabilities.maxImageCount);
        BeeCoreInfo("currentExtent: {}x{}", details.capabilities.currentExtent.width, details.capabilities.currentExtent.height);
        BeeCoreInfo("minImageExtent: {}x{}", details.capabilities.minImageExtent.width, details.capabilities.minImageExtent.height);
        BeeCoreInfo("maxImageExtent: {}x{}", details.capabilities.maxImageExtent.width, details.capabilities.maxImageExtent.height);
        BeeCoreInfo("maxImageArrayLayers: {}", details.capabilities.maxImageArrayLayers);
        BeeCoreInfo("supportedTransforms: {}", vk::to_string(details.capabilities.supportedTransforms));
        BeeCoreInfo("currentTransform: {}", vk::to_string(details.capabilities.currentTransform));
        BeeCoreInfo("supportedCompositeAlpha: {}", vk::to_string(details.capabilities.supportedCompositeAlpha));
        BeeCoreInfo("supportedUsageFlags: {}", vk::to_string(details.capabilities.supportedUsageFlags));

        details.formats = physicalDevice.getSurfaceFormatsKHR(surface);
        BeeCoreInfo("SwapChain can support the following surface formats:");
        for(auto& format : details.formats)
        {
            BeeCoreInfo("format: {}, colorSpace: {}", vk::to_string(format.format), vk::to_string(format.colorSpace));
        }

        details.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
        BeeCoreInfo("SwapChain can support the following surface present modes:");
        for(auto& presentMode : details.presentModes)
        {
            BeeCoreInfo("presentMode: {}", vk::to_string(presentMode));
        }

        return details;
    }

    void VulkanSwapChain::ChooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats)
    {
        for(auto& format : formats)
        {
            if(format.format == vk::Format::eB8G8R8A8Unorm
            && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                m_SurfaceFormat = format;
                return;
            }
        }
        m_SurfaceFormat = formats[0];
    }

    void VulkanSwapChain::ChoosePresentMode(const std::vector<vk::PresentModeKHR>& presentModes)
    {
        for(auto& presentMode : presentModes)
        {
            if(presentMode == vk::PresentModeKHR::eMailbox)
            {
                m_PresentMode = presentMode;
                return;
            }
        }
        m_PresentMode = vk::PresentModeKHR::eFifo;
    }

    void VulkanSwapChain::ChooseExtent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR &capabilities)
    {
        if(capabilities.currentExtent.width != UINT32_MAX)
        {
            m_Extent = capabilities.currentExtent;
        }
        else
        {
            m_Extent = vk::Extent2D(width, height);
            m_Extent.width = std::min(capabilities.minImageExtent.width, std::max(capabilities.maxImageExtent.width, m_Extent.width));
            m_Extent.height = std::min(capabilities.minImageExtent.height, std::max(capabilities.maxImageExtent.height, m_Extent.height));
        }
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
        Destroy();
    }

    void VulkanSwapChain::Recreate()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize((GLFWwindow*)WindowHandler::GetInstance()->GetWindow(), &width, &height);
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize((GLFWwindow*)WindowHandler::GetInstance()->GetWindow(), &width, &height);
            glfwWaitEvents();
        }
        m_LogicalDevice.waitIdle();
        Destroy();
        Create(width, height);
        CreateImageViews();
        CreateFramebuffers();
        CreateCommandBuffers();
        CreateSyncronizationObjects();
    }

    void VulkanSwapChain::Destroy()
    {
        for (auto imageView : m_SwapChainImageViews) {
            m_GraphicsDevice.GetDevice().destroyImageView(imageView, nullptr);
        }
        m_SwapChainImageViews.clear();

        if (m_SwapChain != nullptr) {
            vkDestroySwapchainKHR(m_GraphicsDevice.GetDevice(), m_SwapChain, nullptr);
            m_SwapChain = nullptr;
        }

        for (int i = 0; i < m_DepthImages.size(); i++) {
            vkDestroyImageView(m_GraphicsDevice.GetDevice(), m_DepthImageViews[i], nullptr);
            vkDestroyImage(m_GraphicsDevice.GetDevice(), m_DepthImages[i], nullptr);
            vkFreeMemory(m_GraphicsDevice.GetDevice(), m_DepthImageMemorys[i], nullptr);
        }

        for (auto framebuffer : m_SwapChainFramebuffers) {
            vkDestroyFramebuffer(m_GraphicsDevice.GetDevice(), framebuffer, nullptr);
        }

        vkDestroyRenderPass(m_GraphicsDevice.GetDevice(), m_RenderPass, nullptr);

        // cleanup synchronization objects
        for (size_t i = 0; i < m_MaxFrames; i++) {
            vkDestroySemaphore(m_GraphicsDevice.GetDevice(), m_RenderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(m_GraphicsDevice.GetDevice(), m_ImageAvailableSemaphores[i], nullptr);
            vkDestroyFence(m_GraphicsDevice.GetDevice(), m_InFlightFences[i], nullptr);
        }
    }

    VkResult VulkanSwapChain::AcquireNextImage(uint32_t *imageIndex) {
        vkWaitForFences(
                m_GraphicsDevice.GetDevice(),
                1,
                &m_InFlightFences[m_CurrentFrame],
                VK_TRUE,
                std::numeric_limits<uint64_t>::max());

        VkResult result = vkAcquireNextImageKHR(
                m_GraphicsDevice.GetDevice(),
                m_SwapChain,
                std::numeric_limits<uint64_t>::max(),
                m_ImageAvailableSemaphores[m_CurrentFrame],  // must be a not signaled semaphore
                VK_NULL_HANDLE,
                imageIndex);

        return result;
    }

    VkResult VulkanSwapChain::SubmitCommandBuffers(
            const VkCommandBuffer *buffers, uint32_t *imageIndex) {
        if (m_ImagesInFlight[*imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(m_GraphicsDevice.GetDevice(), 1, &m_ImagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
        }
        m_ImagesInFlight[*imageIndex] = m_InFlightFences[m_CurrentFrame];

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphores[m_CurrentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = buffers;

        VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphores[m_CurrentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(m_GraphicsDevice.GetDevice(), 1, &m_InFlightFences[m_CurrentFrame]);
        vkQueueSubmit(m_GraphicsDevice.GetGraphicsQueue().GetQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame]);

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {m_SwapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = imageIndex;

        auto result = vkQueuePresentKHR(m_GraphicsDevice.GetPresentQueue().GetQueue(), &presentInfo);

        m_CurrentFrame = (m_CurrentFrame + 1) % m_MaxFrames;

        return result;
    }

    void VulkanSwapChain::Create(uint32_t width, uint32_t height)
    {
        m_SwapChainSupportDetails = QuerySwapChainSupport(m_PhysicalDevice, m_Surface);
        ChooseSurfaceFormat(m_SwapChainSupportDetails.formats);
        ChoosePresentMode(m_SwapChainSupportDetails.presentModes);
        ChooseExtent(width, height, m_SwapChainSupportDetails.capabilities);
        //Draw extra image to avoid waiting on the driver
        m_MaxFrames = std::min(m_SwapChainSupportDetails.capabilities.minImageCount + 1,
                               m_SwapChainSupportDetails.capabilities.maxImageCount);
        vk::SwapchainCreateInfoKHR createInfo(
                vk::SwapchainCreateFlagsKHR(),
                m_Surface,
                m_MaxFrames,
                m_SurfaceFormat.format,
                m_SurfaceFormat.colorSpace,
                m_Extent,
                1,
                vk::ImageUsageFlagBits::eColorAttachment
        );

        if(m_QueueFamilyIndices.GraphicsFamily.value() != m_QueueFamilyIndices.PresentFamily.value())
        {
            uint32_t queueFamilyIndicesArray[] = {m_QueueFamilyIndices.GraphicsFamily.value(), m_QueueFamilyIndices.PresentFamily.value()};
            createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndicesArray;
        }
        else
        {
            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        }

        createInfo.preTransform = m_SwapChainSupportDetails.capabilities.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.presentMode = m_PresentMode;
        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = nullptr;

        try
        {
            m_SwapChain = m_LogicalDevice.createSwapchainKHR(createInfo);
        }
        catch(vk::SystemError& e)
        {
            BeeCoreFatalError("Failed to create Vulkan swap chain!");
        }
    }

    void VulkanSwapChain::CreateImageViews()
    {
        auto images = m_LogicalDevice.getSwapchainImagesKHR(m_SwapChain);
        m_Frames.resize(images.size());
        for(size_t i = 0; i < images.size(); i++)
        {
            m_Frames[i].Image = images[i];
            vk::ImageViewCreateInfo createInfo{};
            createInfo.image = images[i];
            createInfo.viewType = vk::ImageViewType::e2D;
            createInfo.components.r = vk::ComponentSwizzle::eIdentity;
            createInfo.components.g = vk::ComponentSwizzle::eIdentity;
            createInfo.components.b = vk::ComponentSwizzle::eIdentity;
            createInfo.components.a = vk::ComponentSwizzle::eIdentity;
            createInfo.format = m_SurfaceFormat.format;
            createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            try
            {
                m_Frames[i].ImageView = m_LogicalDevice.createImageView(createInfo);
            }
            catch(vk::SystemError& e)
            {
                BeeCoreFatalError("Failed to create Vulkan image view!");
            }
        }
    }

    void VulkanSwapChain::CreateSyncronizationObjects()
    {
        for(auto& frame : m_Frames)
        {
            frame.ImageAvailableSemaphore = VulkanSemaphore(m_LogicalDevice);
            frame.RenderFinishedSemaphore = VulkanSemaphore(m_LogicalDevice);
            frame.InFlightFence = VulkanFence(m_LogicalDevice);
        }
    }

    void VulkanSwapChain::CreateFramebuffers()
    {
        auto& graphicsPipeline = (*(VulkanGraphicsDevice*)&WindowHandler::GetInstance()->GetGraphicsDevice()).GetPipeline();
        //VulkanFramebuffer::CreateFramebuffers(m_LogicalDevice, graphicsPipeline.GetRenderPass().GetHandle(), m_Extent, m_Frames);
    }

    void VulkanSwapChain::CreateCommandBuffers()
    {
        auto& commandPool = (*(VulkanGraphicsDevice*)&WindowHandler::GetInstance()->GetGraphicsDevice()).GetCommandPool();

        commandPool.CreateCommandBuffers(m_Frames);
    }

    void VulkanSwapChain::CreateSwapChain()
    {

    }

    void VulkanSwapChain::CreateDepthResources()
    {

    }

    void VulkanSwapChain::CreateRenderPass()
    {

    }

    void VulkanSwapChain::CreateSyncObjects()
    {

    }
}
