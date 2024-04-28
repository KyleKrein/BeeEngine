//
// Created by alexl on 27.01.2023.
//

#pragma once

#include <glm/glm.hpp>
#include "Core/TypeDefines.h"
#include "Core/Color4.h"
#include "CommandBuffer.h"
#include "RenderPass.h"
#include "Model.h"

namespace BeeEngine
{
    class FrameBuffer;
    class RendererAPI {
    public:
        virtual ~RendererAPI() = default;

        virtual void Init() = 0;

        virtual CommandBuffer BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void StartMainCommandBuffer(CommandBuffer& commandBuffer) = 0;
        virtual void EndMainCommandBuffer(CommandBuffer& commandBuffer) = 0;

        [[nodiscard]] virtual CommandBuffer GetCurrentCommandBuffer() = 0;
        virtual void DrawInstanced(CommandBuffer& commandBuffer, Model& model, InstancedBuffer& instancedBuffer, const std::vector<BindingSet*>& bindingSets, uint32_t instanceCount) = 0;
        virtual void SubmitCommandBuffer(const CommandBuffer& commandBuffer) = 0;

        virtual void CopyFrameBufferImageToSwapchain(FrameBuffer& framebuffer, uint32_t attachmentIndex) = 0;

        //virtual void DrawInstanced(const Ref<Model>& model, const Ref<UniformBuffer>& instanceBuffer, uint32_t instanceCount) = 0;

        static Scope<RendererAPI> Create();
    };
}