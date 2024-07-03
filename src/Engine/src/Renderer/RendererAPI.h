//
// Created by alexl on 27.01.2023.
//

#pragma once

#include "CommandBuffer.h"
#include "Core/Color4.h"
#include "Core/Expected.h"
#include "Core/TypeDefines.h"
#include "Model.h"
#include "RenderPass.h"
#include <glm/glm.hpp>

namespace BeeEngine
{
    class FrameBuffer;
    class RendererAPI
    {
    public:
        enum class Error
        {
            SwapchainOutdated
        };
        virtual ~RendererAPI() = default;

        virtual void Init() = 0;

        virtual Expected<CommandBuffer, Error> BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void RebuildSwapchain() = 0;

        virtual void StartMainCommandBuffer(CommandBuffer& commandBuffer) = 0;
        virtual void EndMainCommandBuffer(CommandBuffer& commandBuffer) = 0;

        [[nodiscard]] virtual CommandBuffer GetCurrentCommandBuffer() = 0;
        virtual void DrawInstanced(CommandBuffer& commandBuffer,
                                   Model& model,
                                   InstancedBuffer& instancedBuffer,
                                   const std::vector<BindingSet*>& bindingSets,
                                   uint32_t instanceCount) = 0;
        virtual void SubmitCommandBuffer(const CommandBuffer& commandBuffer) = 0;

        virtual void CopyFrameBufferImageToSwapchain(FrameBuffer& framebuffer, uint32_t attachmentIndex) = 0;

        // virtual void DrawInstanced(const Ref<Model>& model, const Ref<UniformBuffer>& instanceBuffer, uint32_t
        // instanceCount) = 0;

        static Scope<RendererAPI> Create();
    };
} // namespace BeeEngine