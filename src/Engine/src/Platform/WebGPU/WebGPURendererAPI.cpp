//
// Created by Александр Лебедев on 30.06.2023.
//
#if defined(BEE_COMPILE_WEBGPU)
#include "WebGPURendererAPI.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderingQueue.h"
#include "WebGPUGraphicsDevice.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
namespace BeeEngine::Internal
{
    WebGPURendererAPI::WebGPURendererAPI() : m_GraphicsDevice(WebGPUGraphicsDevice::GetInstance()) {}

    WebGPURendererAPI::~WebGPURendererAPI() {}

    CommandBuffer WebGPURendererAPI::BeginFrame()
    {
        wgpuDeviceTick(m_GraphicsDevice.GetDevice());
    Begin:
        if (m_GraphicsDevice.SwapChainRequiresRebuild())
        {
            BeeCoreTrace("Rebuilding swapchain");
            m_GraphicsDevice.WindowResized(0, 0);
        }
        m_NextTexture = wgpuSwapChainGetCurrentTextureView(m_GraphicsDevice.GetSwapChain().GetHandle());
        if (m_NextTexture == nullptr)
        {
            goto Begin;
        }
        m_CurrentCommandBuffer = m_GraphicsDevice.CreateCommandBuffer();
        return m_CurrentCommandBuffer;
    }

    void WebGPURendererAPI::EndFrame()
    {
        SubmitCommandBuffer(m_CurrentCommandBuffer);
        m_GraphicsDevice.SubmitCommandBuffers(m_CommandBuffersForSubmition.data(), m_CommandBuffersForSubmition.size());
        wgpuRenderPassEncoderRelease(m_RenderPassEncoder);
        m_CommandBuffersForSubmition.clear();
        wgpuTextureViewRelease(m_NextTexture);
        m_NextTexture = nullptr;
        wgpuSwapChainPresent(m_GraphicsDevice.GetSwapChain().GetHandle());
    }

    void WebGPURendererAPI::Init() {}

    void WebGPURendererAPI::StartMainRenderPass(BeeEngine::CommandBuffer commandBuffer)
    {
        auto& cmd = *reinterpret_cast<WebGPUCommandBuffer*>(&commandBuffer);
        WGPURenderPassDescriptor renderPassDesc = {};
        renderPassDesc.nextInChain = nullptr;

        WGPURenderPassColorAttachment renderPassColorAttachment = {};
        renderPassColorAttachment.view = m_NextTexture;
        renderPassColorAttachment.resolveTarget = nullptr;
        renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
        renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
        renderPassColorAttachment.clearValue = Renderer::GetClearColor();

        renderPassDesc.colorAttachmentCount = 1;
        renderPassDesc.colorAttachments = &renderPassColorAttachment;

        WGPURenderPassDepthStencilAttachment renderPassDepthStencilAttachment = {};
        renderPassDepthStencilAttachment.view = m_GraphicsDevice.GetSwapChain().GetDepthTextureView();
        renderPassDepthStencilAttachment.depthLoadOp = WGPULoadOp_Clear;
        renderPassDepthStencilAttachment.depthStoreOp = WGPUStoreOp_Store;
        renderPassDepthStencilAttachment.depthClearValue = 1.0f;
        renderPassDepthStencilAttachment.stencilLoadOp = WGPULoadOp_Undefined;
        renderPassDepthStencilAttachment.stencilStoreOp = WGPUStoreOp_Undefined;
        renderPassDepthStencilAttachment.stencilClearValue = 0;
        renderPassDepthStencilAttachment.stencilReadOnly = true;
        renderPassDepthStencilAttachment.depthReadOnly = false;

        renderPassDesc.depthStencilAttachment = &renderPassDepthStencilAttachment;

        renderPassDesc.timestampWriteCount = 0;
        renderPassDesc.timestampWrites = nullptr;

        m_RenderPassEncoder = wgpuCommandEncoderBeginRenderPass(cmd.GetHandle(), &renderPassDesc);
    }

    void WebGPURendererAPI::EndMainRenderPass(BeeEngine::CommandBuffer commandBuffer)
    {
        wgpuRenderPassEncoderEnd(m_RenderPassEncoder);
    }

    RenderPass WebGPURendererAPI::GetMainRenderPass() const
    {
        return {m_RenderPassEncoder};
    }

    void WebGPURendererAPI::DrawInstanced(Model& model,
                                          InstancedBuffer& instancedBuffer,
                                          const std::vector<BindingSet*>& bindingSets,
                                          uint32_t instanceCount)
    {
        model.Bind();
        auto cmd = Renderer::GetCurrentRenderPass();
        instancedBuffer.Bind(&cmd);
        uint32_t index = 0;
        for (auto& bindingSet : bindingSets)
        {
            bindingSet->Bind(&cmd, index++);
        }
        auto renderPass = reinterpret_cast<WGPURenderPassEncoder>(cmd.GetHandle());
        if (model.IsIndexed())
            wgpuRenderPassEncoderDrawIndexed(renderPass, model.GetIndexCount(), instanceCount, 0, 0, 0);
        else
            wgpuRenderPassEncoderDraw(renderPass, model.GetVertexCount(), instanceCount, 0, 0);
    }
} // namespace BeeEngine::Internal
#pragma clang diagnostic pop
#endif