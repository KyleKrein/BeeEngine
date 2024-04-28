//
// Created by Aleksandr on 22.02.2024.
//
#pragma once
#include "Renderer/RendererAPI.h"
#include <vulkan/vulkan.hpp>

#include "VulkanSwapChain.h"
#include "VulkanGraphicsDevice.h"
#include "Renderer/RenderingQueue.h"

namespace BeeEngine::Internal
{
    class VulkanRendererAPI final : public RendererAPI
    {
    public:
        VulkanRendererAPI();
        ~VulkanRendererAPI() override;

        void Init() override;

        CommandBuffer BeginFrame() override;

        void EndFrame() override;

        void StartMainCommandBuffer(CommandBuffer& commandBuffer) override;

        void EndMainCommandBuffer(CommandBuffer& commandBuffer) override;

        [[nodiscard]] CommandBuffer GetCurrentCommandBuffer() override;

        void DrawInstanced(CommandBuffer& commandBuffer, Model& model, InstancedBuffer& instancedBuffer, const std::vector<BindingSet*>& bindingSets,
            uint32_t instanceCount) override;

        void SubmitCommandBuffer(const CommandBuffer& commandBuffer) override;

        void CopyFrameBufferImageToSwapchain(FrameBuffer& framebuffer, uint32_t attachmentIndex) override;
    private:
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void RecreateSwapChain();
    private:
        RenderingQueue m_RenderingQueue;
        std::vector<vk::CommandBuffer> m_CommandBuffers;
        uint32_t m_CurrentImageIndex = 0;
        VulkanGraphicsDevice* m_GraphicsDevice;
        vk::Device m_Device;
        WindowHandler* m_Window;
    };
}
