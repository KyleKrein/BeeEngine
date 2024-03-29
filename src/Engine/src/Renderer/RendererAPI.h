//
// Created by alexl on 27.01.2023.
//

#pragma once

#include <glm/glm.hpp>
#include "Core/TypeDefines.h"
#include "VertexArray.h"
#include "Core/Color4.h"
#include "CommandBuffer.h"
#include "RenderPass.h"
#include "Model.h"

namespace BeeEngine
{
    class RendererAPI {
    public:
        virtual ~RendererAPI() = default;

        virtual void Init() = 0;

        virtual CommandBuffer BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void StartMainRenderPass(CommandBuffer commandBuffer) = 0;
        virtual void EndMainRenderPass(CommandBuffer commandBuffer) = 0;

        [[nodiscard]] virtual RenderPass GetMainRenderPass() const = 0;

        [[nodiscard]] virtual CommandBuffer GetCurrentCommandBuffer() const = 0;

        virtual void SetClearColor(const Color4& color) = 0;
        virtual void Clear() = 0;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) = 0;

        virtual void DrawInstanced(Model& model, InstancedBuffer& instancedBuffer, const std::vector<BindingSet*>& bindingSets, uint32_t instanceCount) = 0;
        virtual void SubmitCommandBuffer(const CommandBuffer& commandBuffer) = 0;

        //virtual void DrawInstanced(const Ref<Model>& model, const Ref<UniformBuffer>& instanceBuffer, uint32_t instanceCount) = 0;

        [[nodiscard]]
        virtual Color4 ReadPixel(uint32_t x, uint32_t y) = 0;
        static Ref<RendererAPI> Create();
    };
}