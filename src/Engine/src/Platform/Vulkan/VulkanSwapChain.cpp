//
// Created by alexl on 09.06.2023.
//
#if defined(BEE_COMPILE_VULKAN)
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

    void VulkanSwapChain::ChooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats)
    {
        for(auto& format : formats)
        {
            if(format.format == vk::Format::eB8G8R8A8Unorm// VK_FORMAT_B8G8R8A8_SRGB might be more correct, but it produces many errors
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
        if(WindowHandler::GetInstance()->GetVSync() == VSync::On)
        {
            m_PresentMode = vk::PresentModeKHR::eFifo;
            BeeCoreInfo("VSync is on");
            return;
        }

        for(auto& presentMode : presentModes)
        {
            if(presentMode == vk::PresentModeKHR::eMailbox)
            {
                m_PresentMode = presentMode;
                BeeCoreInfo("VSync is off");
                return;
            }
        }

        for(auto& presentMode : presentModes)
        {
            if(presentMode == vk::PresentModeKHR::eImmediate)
            {
                m_PresentMode = presentMode;
                BeeCoreInfo("VSync is off: Immediate");
                return;
            }
        }
        BeeCoreInfo("VSync is on");
        m_PresentMode = vk::PresentModeKHR::eFifo;
    }

    void VulkanSwapChain::ChooseExtent(vk::SurfaceCapabilitiesKHR &capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            m_Extent = capabilities.currentExtent;
        } else {
            int width, height;
            SDL_GetWindowSize((SDL_Window*)WindowHandler::GetInstance()->GetWindow(), &width, &height);
            vk::Extent2D actualExtent = vk::Extent2D {(uint32_t)width, (uint32_t)height};
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

    vk::Result VulkanSwapChain::AcquireNextImage(uint32_t *imageIndex) {
        auto device = m_GraphicsDevice.GetDevice();
        device.waitForFences(
            1,
            &m_InFlightFences[m_CurrentFrame],
            vk::True,
            std::numeric_limits<uint64_t>::max());

        vk::Result result = device.acquireNextImageKHR(
                m_SwapChain,
                std::numeric_limits<uint64_t>::max(),
                m_ImageAvailableSemaphores[m_CurrentFrame],  // must be a not signaled semaphore
                nullptr,
                imageIndex);

        return result;
    }

    vk::Result VulkanSwapChain::SubmitCommandBuffers(
            const vk::CommandBuffer *buffers, uint32_t *imageIndex) {
        auto device = m_GraphicsDevice.GetDevice();
        if (m_ImagesInFlight[*imageIndex] != VK_NULL_HANDLE) {
            device.waitForFences(1, &m_ImagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
        }
        m_ImagesInFlight[*imageIndex] = m_InFlightFences[m_CurrentFrame];

        vk::SubmitInfo submitInfo = {};
        submitInfo.sType = vk::StructureType::eSubmitInfo;

        vk::Semaphore waitSemaphores[] = {m_ImageAvailableSemaphores[m_CurrentFrame]};
        vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = buffers;

        vk::Semaphore signalSemaphores[] = {m_RenderFinishedSemaphores[m_CurrentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        device.resetFences(1, &m_InFlightFences[m_CurrentFrame]);
        auto queue = m_GraphicsDevice.GetGraphicsQueue();
        queue.submit(1, &submitInfo, m_InFlightFences[m_CurrentFrame]);

        vk::PresentInfoKHR presentInfo = {};
        presentInfo.sType = vk::StructureType::ePresentInfoKHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        vk::SwapchainKHR swapChains[] = {m_SwapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = imageIndex;

        auto result = queue.presentKHR(&presentInfo);

        m_CurrentFrame = (m_CurrentFrame + 1) % m_MaxFrames;

        return result;
    }

    void VulkanSwapChain::CreateImageViews()
    {
        m_SwapChainImageViews.resize(m_SwapChainImages.size());
        for (size_t i = 0; i < m_SwapChainImages.size(); i++) {
            vk::ImageViewCreateInfo viewInfo{};
            viewInfo.sType = vk::StructureType::eImageViewCreateInfo;
            viewInfo.image = m_SwapChainImages[i];
            viewInfo.viewType = vk::ImageViewType::e2D;
            viewInfo.format = m_SurfaceFormat.format;
            viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            vk::Result result = m_GraphicsDevice.GetDevice().createImageView(&viewInfo, nullptr, &m_SwapChainImageViews[i]);
            if (result != vk::Result::eSuccess) {
                throw std::runtime_error("failed to create image views!");
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

        vk::SwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = vk::StructureType::eSwapchainCreateInfoKHR;
        createInfo.surface = m_GraphicsDevice.GetSurface();

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = m_SurfaceFormat.format;
        createInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
        createInfo.imageExtent = m_Extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

        QueueFamilyIndices indices = m_GraphicsDevice.GetQueueFamilyIndices();
        uint32_t queueFamilyIndices[] = {indices.GraphicsFamily.value(), indices.GraphicsFamily.value()};

        if (indices.PresentFamily.value() != indices.GraphicsFamily.value()) {
            createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
            createInfo.queueFamilyIndexCount = 0;      // Optional
            createInfo.pQueueFamilyIndices = nullptr;  // Optional
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

        createInfo.presentMode = m_PresentMode;
        createInfo.clipped = vk::True;

        createInfo.oldSwapchain = m_SwapChain;

        auto device = m_GraphicsDevice.GetDevice();
        vk::Result result = device.createSwapchainKHR(&createInfo, nullptr, &m_SwapChain);
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("failed to create swap chain!");
        }
        // we only specified a minimum number of images in the swap chain, so the implementation is
        // allowed to create a swap chain with more. That's why we'll first query the final number of
        // images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
        // retrieve the handles.
        device.getSwapchainImagesKHR(m_SwapChain, &imageCount, nullptr);
        m_SwapChainImages.resize(imageCount);
        device.getSwapchainImagesKHR(m_SwapChain, &imageCount, m_SwapChainImages.data());
    }

    void VulkanSwapChain::CreateDepthResources()
    {
        vk::Format depthFormat = FindDepthFormat();
        vk::Extent2D swapChainExtent = m_Extent;
        size_t size = ImageCount();
        m_DepthImages.resize(size);
        m_DepthImageViews.resize(size);

        size = m_DepthImages.size();

        for (int i = 0; i < size; i++) {
            vk::ImageCreateInfo imageInfo{};
            imageInfo.sType = vk::StructureType::eImageCreateInfo;
            imageInfo.imageType = vk::ImageType::e2D;
            imageInfo.extent.width = swapChainExtent.width;
            imageInfo.extent.height = swapChainExtent.height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = depthFormat;
            imageInfo.tiling = vk::ImageTiling::eOptimal;
            imageInfo.initialLayout = vk::ImageLayout::eUndefined;
            imageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
            imageInfo.samples = vk::SampleCountFlagBits::e1;
            imageInfo.sharingMode = vk::SharingMode::eExclusive;
            imageInfo.flags = {};

            vk::ImageViewCreateInfo viewInfo{};
            viewInfo.sType = vk::StructureType::eImageViewCreateInfo;
            viewInfo.viewType = vk::ImageViewType::e2D;
            viewInfo.format = depthFormat;
            viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            m_GraphicsDevice.CreateImageWithInfo(
                    imageInfo,
                    viewInfo,
                    vk::MemoryPropertyFlagBits::eDeviceLocal,
                    VMA_MEMORY_USAGE_GPU_ONLY,
                    m_DepthImages[i],
                    m_DepthImageViews[i]);
        }
    }

    void VulkanSwapChain::CreateRenderPass()
    {
        vk::AttachmentDescription colorAttachment = {};
        colorAttachment.format = m_SurfaceFormat.format;
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        vk::AttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::AttachmentDescription depthAttachment{};
        depthAttachment.flags = {};
        depthAttachment.format = FindDepthFormat();
        depthAttachment.samples = vk::SampleCountFlagBits::e1;
        depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
        depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::AttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::SubpassDescription subpass = {};
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        vk::SubpassDependency dependency = {};
        dependency.srcSubpass = vk::SubpassExternal;
        dependency.srcAccessMask = {};
        dependency.srcStageMask =
                vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.dstSubpass = 0;
        dependency.dstStageMask =
                vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.dstAccessMask =
                vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

        vk::SubpassDependency depthDependency = {};
        depthDependency.srcSubpass = vk::SubpassExternal;
        depthDependency.dstSubpass = 0;
        depthDependency.srcStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
        depthDependency.srcAccessMask = {};
        depthDependency.dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
        depthDependency.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;

        std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
        std::array<vk::SubpassDependency, 2> dependencies = {dependency, depthDependency};
        vk::RenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = vk::StructureType::eRenderPassCreateInfo;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = dependencies.size();
        renderPassInfo.pDependencies = dependencies.data();

        if (m_GraphicsDevice.GetDevice().createRenderPass(&renderPassInfo, nullptr, &m_RenderPass) != vk::Result::eSuccess) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void VulkanSwapChain::CreateSyncObjects()
    {
        m_ImageAvailableSemaphores.resize(m_MaxFrames);
        m_RenderFinishedSemaphores.resize(m_MaxFrames);
        m_InFlightFences.resize(m_MaxFrames);
        m_ImagesInFlight.resize(ImageCount(), VK_NULL_HANDLE);

        vk::SemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = vk::StructureType::eSemaphoreCreateInfo;

        vk::FenceCreateInfo fenceInfo = {};
        fenceInfo.sType = vk::StructureType::eFenceCreateInfo;
        fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
        auto device = m_GraphicsDevice.GetDevice();
        for (size_t i = 0; i < m_MaxFrames; i++) {
            if (device.createSemaphore(&semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) !=
                vk::Result::eSuccess ||
                device.createSemaphore(&semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) !=
                vk::Result::eSuccess ||
                device.createFence(&fenceInfo, nullptr, &m_InFlightFences[i]) != vk::Result::eSuccess) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    void VulkanSwapChain::CreateFramebuffers()
    {
        size_t size = ImageCount();
        m_SwapChainFramebuffers.resize(size);
        auto device = m_GraphicsDevice.GetDevice();
        for (size_t i = 0; i < size; i++) {
            std::array<vk::ImageView, 2> attachments = {m_SwapChainImageViews[i], m_DepthImageViews[i]};

            vk::Extent2D swapChainExtent = m_Extent;
            vk::FramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = vk::StructureType::eFramebufferCreateInfo;
            framebufferInfo.renderPass = m_RenderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (device.createFramebuffer(
                    &framebufferInfo,
                    nullptr,
                    &m_SwapChainFramebuffers[i]) != vk::Result::eSuccess) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    vk::Format VulkanSwapChain::FindDepthFormat()
    {
        return m_GraphicsDevice.FindSupportedFormat(
                {vk::Format::eD32Sfloat/*, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT*/},
                vk::ImageTiling::eOptimal,
                vk::FormatFeatureFlagBits::eDepthStencilAttachment);
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
#endif