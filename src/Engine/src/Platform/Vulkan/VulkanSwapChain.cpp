//
// Created by alexl on 09.06.2023.
//

#include "VulkanSwapChain.h"
#include "Core/Logging/Log.h"
#include "shaderc/shaderc.hpp"
#include "Windowing/WindowHandler/WindowHandler.h"
#include "VulkanGraphicsDevice.h"
#include "SDL3/SDL_vulkan.h"


namespace BeeEngine::Internal
{

    VulkanSwapChain::VulkanSwapChain(VulkanGraphicsDevice& graphicsDevice, uint32_t width, uint32_t height, VkSwapchainKHR oldSwapChain)
    : m_GraphicsDevice(graphicsDevice), m_Extent(vk::Extent2D(width, height)), m_SwapChain(oldSwapChain)
    {
        CreateSwapChain();
        CreateImageViews();
        CreateRenderPass();
        CreateDepthResources();
        CreateFramebuffers();
        //CreateCommandBuffers();
        CreateSyncObjects();
    }

    void VulkanSwapChain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
    {
        for(auto& format : formats)
        {
            if(format.format == VK_FORMAT_B8G8R8A8_UNORM// VK_FORMAT_B8G8R8A8_SRGB might be more correct, but it produces many errors
            && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
            {
                m_SurfaceFormat = format;
                return;
            }
        }
        m_SurfaceFormat = formats[0];
    }

    void VulkanSwapChain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes)
    {
        if(WindowHandler::GetInstance()->GetVSync() == VSync::On)
        {
            m_PresentMode = VK_PRESENT_MODE_FIFO_KHR;
            BeeCoreInfo("VSync is on");
            return;
        }

        for(auto& presentMode : presentModes)
        {
            if(presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                m_PresentMode = presentMode;
                BeeCoreInfo("VSync is off");
                return;
            }
        }

        for(auto& presentMode : presentModes)
        {
            if(presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            {
                m_PresentMode = presentMode;
                BeeCoreInfo("VSync is off: Immediate");
                return;
            }
        }
        BeeCoreInfo("VSync is on");
        m_PresentMode = VK_PRESENT_MODE_FIFO_KHR;
    }

    void VulkanSwapChain::ChooseExtent(VkSurfaceCapabilitiesKHR &capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            m_Extent = capabilities.currentExtent;
        } else {
            int width, height;
#if defined(DESKTOP_PLATFORM)
            if(WindowHandler::GetAPI() == WindowHandlerAPI::SDL)
            {
#endif
                SDL_GetWindowSize((SDL_Window*)WindowHandler::GetInstance()->GetWindow(), &width, &height);
#if defined(DESKTOP_PLATFORM)
            }
            else
            {
                glfwGetFramebufferSize((GLFWwindow*)WindowHandler::GetInstance()->GetWindow(), &width, &height);
            }
#endif
            VkExtent2D actualExtent = VkExtent2D {(uint32_t)width, (uint32_t)height};
            actualExtent.width = std::max(
                    capabilities.minImageExtent.width,
                    std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(
                    capabilities.minImageExtent.height,
                    std::min(capabilities.maxImageExtent.height, actualExtent.height));

            m_Extent = actualExtent;
        }
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
        Destroy();
    }

    void VulkanSwapChain::Destroy()
    {
        BeeCoreTrace("Destroying SwapChain");
        vkDeviceWaitIdle(m_GraphicsDevice.GetDevice());
        for (auto imageView : m_SwapChainImageViews) {
            m_GraphicsDevice.GetDevice().destroyImageView(imageView, nullptr);
        }
        m_SwapChainImageViews.clear();

        if (m_SwapChain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(m_GraphicsDevice.GetDevice(), m_SwapChain, nullptr);
            //m_SwapChain = nullptr;
        }

        for(auto& depthImage : m_DepthImages)
        {
            //vmaDestroyImage(GetVulkanAllocator(), depthImage.Image, depthImage.Memory);
        }

        for(auto depthImageView : m_DepthImageViews)
        {
            //m_GraphicsDevice.GetDevice().destroyImageView(depthImageView, nullptr);
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

    void VulkanSwapChain::CreateImageViews()
    {
        m_SwapChainImageViews.resize(m_SwapChainImages.size());
        for (size_t i = 0; i < m_SwapChainImages.size(); i++) {
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = m_SwapChainImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = m_SurfaceFormat.format;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_GraphicsDevice.GetDevice(), &viewInfo, nullptr, &m_SwapChainImageViews[i]) !=
                VK_SUCCESS) {
                throw std::runtime_error("failed to create texture image view!");
            }
        }
    }

    void VulkanSwapChain::CreateSwapChain()
    {
        SwapChainSupportDetails swapChainSupport = m_GraphicsDevice.GetSwapChainSupportDetails();
        
        m_MaxFrames = swapChainSupport.capabilities.maxImageCount;

        ChooseSurfaceFormat(swapChainSupport.formats);
        ChoosePresentMode(swapChainSupport.presentModes);
        ChooseExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_GraphicsDevice.GetSurface().GetHandle();

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = m_SurfaceFormat.format;
        createInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
        createInfo.imageExtent = m_Extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = m_GraphicsDevice.GetQueueFamilyIndices();
        uint32_t queueFamilyIndices[] = {indices.GraphicsFamily.value(), indices.GraphicsFamily.value()};

        if (indices.GraphicsFamily.value() != indices.GraphicsFamily.value()) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;      // Optional
            createInfo.pQueueFamilyIndices = nullptr;  // Optional
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        createInfo.presentMode = m_PresentMode;
        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = m_SwapChain;

        if (vkCreateSwapchainKHR(m_GraphicsDevice.GetDevice(), &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }
        // we only specified a minimum number of images in the swap chain, so the implementation is
        // allowed to create a swap chain with more. That's why we'll first query the final number of
        // images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
        // retrieve the handles.
        vkGetSwapchainImagesKHR(m_GraphicsDevice.GetDevice(), m_SwapChain, &imageCount, nullptr);
        m_SwapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_GraphicsDevice.GetDevice(), m_SwapChain, &imageCount, m_SwapChainImages.data());
    }

    void VulkanSwapChain::CreateDepthResources()
    {
        VkFormat depthFormat = FindDepthFormat();
        VkExtent2D swapChainExtent = m_Extent;
        size_t size = ImageCount();
        m_DepthImages.resize(size);
        m_DepthImageViews.resize(size);

        size = m_DepthImages.size();

        for (int i = 0; i < size; i++) {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = swapChainExtent.width;
            imageInfo.extent.height = swapChainExtent.height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = depthFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.flags = 0;

            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = depthFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            m_GraphicsDevice.CreateImageWithInfo(
                    imageInfo,
                    viewInfo,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    VMA_MEMORY_USAGE_GPU_ONLY,
                    m_DepthImages[i],
                    m_DepthImageViews[i]);
        }
    }

    void VulkanSwapChain::CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = m_SurfaceFormat.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.flags = 0;
        depthAttachment.format = FindDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.srcAccessMask = 0;
        dependency.srcStageMask =
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstSubpass = 0;
        dependency.dstStageMask =
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask =
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        VkSubpassDependency depthDependency = {};
        depthDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        depthDependency.dstSubpass = 0;
        depthDependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        depthDependency.srcAccessMask = 0;
        depthDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        depthDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
        std::array<VkSubpassDependency, 2> dependencies = {dependency, depthDependency};
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = dependencies.size();
        renderPassInfo.pDependencies = dependencies.data();

        if (vkCreateRenderPass(m_GraphicsDevice.GetDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void VulkanSwapChain::CreateSyncObjects()
    {
        m_ImageAvailableSemaphores.resize(m_MaxFrames);
        m_RenderFinishedSemaphores.resize(m_MaxFrames);
        m_InFlightFences.resize(m_MaxFrames);
        m_ImagesInFlight.resize(ImageCount(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < m_MaxFrames; i++) {
            if (vkCreateSemaphore(m_GraphicsDevice.GetDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) !=
                VK_SUCCESS ||
                vkCreateSemaphore(m_GraphicsDevice.GetDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) !=
                VK_SUCCESS ||
                vkCreateFence(m_GraphicsDevice.GetDevice(), &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    void VulkanSwapChain::CreateFramebuffers()
    {
        size_t size = ImageCount();
        m_SwapChainFramebuffers.resize(size);
        for (size_t i = 0; i < size; i++) {
            std::array<VkImageView, 2> attachments = {m_SwapChainImageViews[i], m_DepthImageViews[i]};

            VkExtent2D swapChainExtent = m_Extent;
            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_RenderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(
                    m_GraphicsDevice.GetDevice(),
                    &framebufferInfo,
                    nullptr,
                    &m_SwapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    VkFormat VulkanSwapChain::FindDepthFormat()
    {
        return m_GraphicsDevice.FindSupportedFormat(
                {VK_FORMAT_D32_SFLOAT/*, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT*/},
                VK_IMAGE_TILING_OPTIMAL,
                VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

    size_t VulkanSwapChain::ImageCount()
    {
        return m_SwapChainImages.size();
    }

    vk::RenderPass VulkanSwapChain::GetRenderPass()
    {
        return m_RenderPass;
    }

    bool VulkanSwapChain::ResizeInProgress()
    {
        return m_ResizeInProgress;
    }
}
