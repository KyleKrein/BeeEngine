//
// Created by Александр Лебедев on 24.06.2023.
//

#include "VulkanRendererAPI.h"
#include "SDL3/SDL_vulkan.h"
#include "SDL3/SDL.h"

namespace BeeEngine::Internal
{
    VulkanRendererAPI::VulkanRendererAPI()
    : m_GraphicsDevice((*(BeeEngine::Internal::VulkanGraphicsDevice*)&BeeEngine::WindowHandler::GetInstance()->GetGraphicsDevice()))
    {
        CreateCommandBuffers();
    }

    VulkanRendererAPI::~VulkanRendererAPI()
    {
        //FreeCommandBuffers();
    }

    void VulkanRendererAPI::CreateCommandBuffers()
    {
        m_CommandBuffers.resize(m_GraphicsDevice.GetSwapChain().ImageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_GraphicsDevice.GetCommandPool().GetHandle();
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

        if(vkAllocateCommandBuffers(m_GraphicsDevice.GetDevice(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
        {
            BeeError("Failed to allocate command buffers");
        }
    }

    void VulkanRendererAPI::RecreateSwapchain()
    {
        BeeCoreInfo("Recreating swap chain");
        int width = 0, height = 0;
        /*
        //SDL_GetWindowSizeInPixels((SDL_Window*)WindowHandler::GetInstance()->GetWindow(), &width, &height);
        if (width == 0 || height == 0)
        {
#if defined(DESKTOP_PLATFORM) && defined(BEE_COMPILE_GLFW)
            if(WindowHandler::GetAPI() == WindowHandlerAPI::SDL)
            {
#endif
                SDL_GetWindowSizeInPixels((SDL_Window*)WindowHandler::GetInstance()->GetWindow(), &width, &height);
                SDL_PumpEvents();
                //WindowHandler::GetInstance()->ProcessEvents();
#if defined(DESKTOP_PLATFORM) && defined(BEE_COMPILE_GLFW)
            }
            else
            {
                glfwGetWindowSize((GLFWwindow*)WindowHandler::GetInstance()->GetWindow(), &width, &height);
                glfwWaitEvents();
            }
#endif
        }
*/
        m_GraphicsDevice.WindowResized(width, height);
        if(m_GraphicsDevice.GetSwapChain().ImageCount() != m_CommandBuffers.size())
        {
            FreeCommandBuffers();
            CreateCommandBuffers();
        }
        //TODO: Check if the renderpass is compatible with the new swapchain
        //CreatePipeline();
    }

    void VulkanRendererAPI::FreeCommandBuffers()
    {
        auto& device = (*(BeeEngine::Internal::VulkanGraphicsDevice*)&BeeEngine::WindowHandler::GetInstance()->GetGraphicsDevice());
        vkFreeCommandBuffers(device.GetDevice(), device.GetCommandPool().GetHandle(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
        m_CommandBuffers.clear();
    }

    VkCommandBuffer VulkanRendererAPI::BeginFrame()
    {
        BeginFrame:
        BeeCoreAssert(!m_IsFrameStarted, "Can't call BeginFrame while already in frame");

        if(m_GraphicsDevice.SwapChainRequiresRebuild())
        {
            auto oldSwapChainptr = &m_GraphicsDevice.GetSwapChain();
            RecreateSwapchain();
            BeeEnsures(oldSwapChainptr != &m_GraphicsDevice.GetSwapChain());
        }

        auto result = m_GraphicsDevice.GetSwapChain().AcquireNextImage(&m_CurrentImageIndex);
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            m_GraphicsDevice.RequestSwapChainRebuild();
            goto BeginFrame;
        }
        if(result != VK_SUCCESS)
        {
            BeeCoreError("Failed to acquire swap chain image");
        }
        m_IsFrameStarted = true;
        auto commandBuffer = GetCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            BeeCoreError("Failed to begin recording command buffer");
        }
        return commandBuffer;
    }

    void VulkanRendererAPI::EndFrame()
    {
        BeeCoreAssert(m_IsFrameStarted, "Can't call EndFrame outside of a frame");
        auto commandBuffer = GetCurrentCommandBuffer();
        if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            BeeCoreError("Failed to record command buffer");
        }
        auto result = m_GraphicsDevice.GetSwapChain().SubmitCommandBuffers(&commandBuffer, &m_CurrentImageIndex);
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            m_GraphicsDevice.RequestSwapChainRebuild();
        }
        else if(result != VK_SUCCESS)
        {
            BeeCoreError("Failed to present swap chain image");
        }
        m_IsFrameStarted = false;
    }

    void VulkanRendererAPI::BeginSwapchainRenderPass(VkCommandBuffer commandBuffer)
    {
        BeeCoreAssert(m_IsFrameStarted, "Can't call BeginSwapchainRenderPass outside of a frame");
        BeeExpects(commandBuffer == GetCurrentCommandBuffer());

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_GraphicsDevice.GetSwapChain().GetRenderPass();
        renderPassInfo.framebuffer = m_GraphicsDevice.GetSwapChain().GetFrameBuffer(m_CurrentImageIndex);

        renderPassInfo.renderArea.offset = {0,0};
        renderPassInfo.renderArea.extent = m_GraphicsDevice.GetSwapChain().GetExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil.depth = 1.f; //= {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkQueueWaitIdle(m_GraphicsDevice.GetGraphicsQueue().GetQueue());
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)m_GraphicsDevice.GetSwapChain().GetExtent().width;
        viewport.height = (float)m_GraphicsDevice.GetSwapChain().GetExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{};
        scissor.offset = {0,0};
        scissor.extent = m_GraphicsDevice.GetSwapChain().GetExtent();
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void VulkanRendererAPI::EndSwapchainRenderPass(VkCommandBuffer commandBuffer)
    {
        BeeCoreAssert(m_IsFrameStarted, "Can't call EndSwapchainRenderPass outside of a frame");
        BeeExpects(commandBuffer == GetCurrentCommandBuffer());

        vkCmdEndRenderPass(commandBuffer);
    }
}