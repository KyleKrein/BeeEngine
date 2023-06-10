//
// Created by alexl on 10.06.2023.
//

#include "VulkanRendererAPI.h"
#include "VulkanGraphicsDevice.h"
#include "vulkan/vulkan.h"


namespace BeeEngine::Internal
{
    VulkanRendererAPI::VulkanRendererAPI()
    : m_GraphicsDevice(*(VulkanGraphicsDevice*)&WindowHandler::GetInstance()->GetGraphicsDevice()),
    m_Device(m_GraphicsDevice.GetDevice()),
    m_SwapChainFrames(m_GraphicsDevice.GetSwapChain().GetFrames()),
    m_MaxFramesInFlight(gsl::narrow_cast<uint32_t>(m_GraphicsDevice.GetSwapChain().GetFrames().size()))
    {
    }

    void VulkanRendererAPI::Render()
    {
        m_Device.waitForFences(m_SwapChainFrames[m_CurrentFrame].InFlightFence.GetHandle(), VK_TRUE, UINT64_MAX);
        m_Device.resetFences(m_SwapChainFrames[m_CurrentFrame].InFlightFence.GetHandle());
        uint32_t imageIndex = -1;
        auto result = vkAcquireNextImageKHR(m_Device, m_GraphicsDevice.GetSwapChain().GetHandle(), UINT64_MAX, m_SwapChainFrames[m_CurrentFrame].ImageAvailableSemaphore.GetHandle(), nullptr, &imageIndex);
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_INCOMPATIBLE_DISPLAY_KHR)
        {
            m_GraphicsDevice.GetSwapChain().Recreate();
            return;
        }
        else if(result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        auto commandBuffer = m_GraphicsDevice.GetSwapChain().GetFrames()[m_CurrentFrame].CommandBuffer;
        commandBuffer.GetHandle().reset();
        commandBuffer.RecordDrawCommands(m_GraphicsDevice.GetPipeline().GetRenderPass(), m_GraphicsDevice.GetSwapChain().GetExtent(), m_GraphicsDevice.GetPipeline().GetHandle());
        vk::SubmitInfo submitInfo;
        vk::Semaphore waitSemaphores[] = {m_SwapChainFrames[m_CurrentFrame].ImageAvailableSemaphore.GetHandle()};
        vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        submitInfo.setWaitSemaphoreCount(1);
        submitInfo.setPWaitSemaphores(waitSemaphores);
        submitInfo.setPWaitDstStageMask(waitStages);
        submitInfo.setCommandBufferCount(1);
        submitInfo.setPCommandBuffers(&commandBuffer.GetHandle());
        vk::Semaphore signalSemaphores[] = {m_SwapChainFrames[m_CurrentFrame].RenderFinishedSemaphore.GetHandle()};
        submitInfo.setSignalSemaphoreCount(1);
        submitInfo.setPSignalSemaphores(signalSemaphores);

        //ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer.GetHandle());

        m_GraphicsDevice.GetGraphicsQueue().GetQueue().submit(submitInfo, m_SwapChainFrames[m_CurrentFrame].InFlightFence.GetHandle());

        vk::PresentInfoKHR presentInfo;
        presentInfo.setWaitSemaphoreCount(1);
        presentInfo.setPWaitSemaphores(signalSemaphores);
        vk::SwapchainKHR swapChains[] = {m_GraphicsDevice.GetSwapChain().GetHandle()};
        presentInfo.setSwapchainCount(1);
        presentInfo.setPSwapchains(swapChains);
        presentInfo.setPImageIndices(&imageIndex);

        result = vkQueuePresentKHR(m_GraphicsDevice.GetPresentQueue().GetQueue(),
                                   reinterpret_cast<const VkPresentInfoKHR *>(&presentInfo));
        //m_GraphicsDevice.GetPresentQueue().GetQueue().presentKHR(presentInfo);

        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_INCOMPATIBLE_DISPLAY_KHR)
        {
            m_GraphicsDevice.GetSwapChain().Recreate();
            return;
        }
        else if(result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        m_CurrentFrame = (m_CurrentFrame + 1) % m_MaxFramesInFlight;
        /*
        vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        vk::SubmitInfo submitInfo = {
                1,
                &m_ImageAvailableSemaphore.GetHandle(),
                waitStages,
                1,
                &m_GraphicsDevice.GetSwapChain().GetFrames()[imageIndex].CommandBuffer.GetHandle(),
                1,
                &m_RenderFinishedSemaphore.GetHandle()
        };
        m_GraphicsDevice.GetGraphicsQueue().GetQueue().submit(submitInfo, m_InFlightFence.GetHandle());
        vk::PresentInfoKHR presentInfo = {
                1,
                &m_RenderFinishedSemaphore.GetHandle(),
                1,
                &m_GraphicsDevice.GetSwapChain().GetHandle(),
                &imageIndex
        };
        m_GraphicsDevice.GetPresentQueue().GetQueue().presentKHR(presentInfo);
         */
    }
}
