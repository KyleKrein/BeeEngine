//
// Created by Александр Лебедев on 30.06.2023.
//

#pragma once
#if defined(BEE_COMPILE_WEBGPU)
#include "Renderer/RendererAPI.h"
#include <webgpu/webgpu.h>

namespace BeeEngine::Internal
{
    class WebGPURendererAPI : public RendererAPI
    {
    public:
        WebGPURendererAPI();
        ~WebGPURendererAPI() override;

        void Init() override;

        CommandBuffer BeginFrame() override;
        void EndFrame() override;

        void StartMainRenderPass(CommandBuffer commandBuffer) override;
        void EndMainRenderPass(CommandBuffer commandBuffer) override;

        [[nodiscard]] RenderPass GetMainRenderPass() const override;

        [[nodiscard]] CommandBuffer GetCurrentCommandBuffer() const override { return m_CurrentCommandBuffer; }

        void DrawInstanced(Model& model,
                           InstancedBuffer& instancedBuffer,
                           const std::vector<BindingSet*>& bindingSets,
                           uint32_t instanceCount) override;

        void SubmitCommandBuffer(const CommandBuffer& commandBuffer) override
        {
            m_CommandBuffersForSubmition.push_back(commandBuffer);
        }

    private:
        class WebGPUGraphicsDevice& m_GraphicsDevice;
        WGPUTextureView m_NextTexture;
        CommandBuffer m_CurrentCommandBuffer;
        WGPURenderPassEncoder m_RenderPassEncoder;

        std::vector<CommandBuffer> m_CommandBuffersForSubmition;
    };
} // namespace BeeEngine::Internal
#endif