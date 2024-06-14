//
// Created by Aleksandr on 22.02.2024.
//
#pragma once
#include "Core/Expected.h"
#include "Renderer/CommandBuffer.h"
#include "Renderer/RendererAPI.h"
#include <vulkan/vulkan.hpp>

#include "Renderer/RenderingQueue.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanSwapChain.h"

namespace BeeEngine::Internal
{
    class VulkanRendererAPI final : public RendererAPI
    {
    public:
        VulkanRendererAPI();
        ~VulkanRendererAPI() override;

        void Init() override;

        Expected<CommandBuffer, RendererAPI::Error> BeginFrame() override;

        void EndFrame() override;

        void StartMainCommandBuffer(CommandBuffer& commandBuffer) override;

        void EndMainCommandBuffer(CommandBuffer& commandBuffer) override;

        [[nodiscard]] CommandBuffer GetCurrentCommandBuffer() override;

        void DrawInstanced(CommandBuffer& commandBuffer,
                           Model& model,
                           InstancedBuffer& instancedBuffer,
                           const std::vector<BindingSet*>& bindingSets,
                           uint32_t instanceCount) override;

        void SubmitCommandBuffer(const CommandBuffer& commandBuffer) override;

        void CopyFrameBufferImageToSwapchain(FrameBuffer& framebuffer, uint32_t attachmentIndex) override;
        void RebuildSwapchain() override;

    private:
        void CreateCommandBuffers();
        void FreeCommandBuffers();

    private:
        RenderingQueue m_RenderingQueue;
        std::vector<vk::CommandBuffer> m_CommandBuffers;
        uint32_t m_CurrentImageIndex = 0;
        VulkanGraphicsDevice* m_GraphicsDevice;
        vk::Device m_Device;
        WindowHandler* m_Window;
    };
} // namespace BeeEngine::Internal
