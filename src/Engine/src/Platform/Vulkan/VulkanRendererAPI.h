//
// Created by Александр Лебедев on 24.06.2023.
//

#pragma once
#include "vulkan/vulkan.h"
#include "Core/TypeDefines.h"
#include "Platform/Vulkan/VulkanGraphicsDevice.h"

namespace BeeEngine::Internal
{
    class VulkanRendererAPI
    {
    public:
        VulkanRendererAPI();
        ~VulkanRendererAPI();

        VkCommandBuffer BeginFrame();
        void EndFrame();
        void BeginSwapchainRenderPass(VkCommandBuffer commandBuffer);
        void EndSwapchainRenderPass(VkCommandBuffer commandBuffer);

        bool IsFrameStarted() const
        {
            return m_IsFrameStarted;
        }
        VkCommandBuffer GetCurrentCommandBuffer() const
        {
            BeeExpects(m_IsFrameStarted);
            return m_CommandBuffers[m_CurrentImageIndex];
        }

        VkRenderPass GetSwapchainRenderPass() const
        {
            return m_GraphicsDevice.GetSwapChain().GetRenderPass();
        }

        VulkanRendererAPI(const VulkanRendererAPI& other) = delete;
        VulkanRendererAPI& operator=(const VulkanRendererAPI& other) = delete;
    private:
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void RecreateSwapchain();

    private:
        std::vector<VkCommandBuffer> m_CommandBuffers;
        uint32_t m_CurrentImageIndex = 0;
        bool m_IsFrameStarted = false;
        VulkanGraphicsDevice& m_GraphicsDevice;
    };
}
