//
// Created by Александр Лебедев on 30.06.2023.
//

#pragma once
#include <webgpu/webgpu.h>
#include "Renderer/RendererAPI.h"

namespace BeeEngine::Internal
{
    class WebGPURendererAPI: public RendererAPI
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

        void SetClearColor(const Color4& color) override;
        void Clear() override;

        [[nodiscard]]CommandBuffer GetCurrentCommandBuffer() const override { return m_CurrentCommandBuffer; }

        void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) override;
        void DrawInstanced(Model& model, InstancedBuffer& instancedBuffer, std::vector<BindingSet*>& bindingSets, uint32_t instanceCount) override;

        [[nodiscard]] Color4 ReadPixel(uint32_t x, uint32_t y) override;
    private:
        class WebGPUGraphicsDevice& m_GraphicsDevice;
        WGPUTextureView m_NextTexture;
        CommandBuffer m_CurrentCommandBuffer;
        WGPURenderPassEncoder m_RenderPassEncoder;
    };
}
