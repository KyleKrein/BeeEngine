//
// Created by Александр Лебедев on 30.06.2023.
//

#include "WebGPURendererAPI.h"
#include "WebGPUGraphicsDevice.h"
#include "Renderer/Renderer.h"

namespace BeeEngine::Internal
{
    WebGPURendererAPI::WebGPURendererAPI()
            : m_GraphicsDevice(WebGPUGraphicsDevice::GetInstance())
    {

    }

    WebGPURendererAPI::~WebGPURendererAPI()
    {

    }

    CommandBuffer WebGPURendererAPI::BeginFrame()
    {
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
        m_GraphicsDevice.SubmitCommandBuffers(&m_CurrentCommandBuffer, 1);//TODO: make it work with multiple command buffers

        wgpuTextureViewRelease(m_NextTexture);
        m_NextTexture = nullptr;
        wgpuSwapChainPresent(m_GraphicsDevice.GetSwapChain().GetHandle());
    }

    void WebGPURendererAPI::Init()
    {

    }

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

        renderPassDesc.depthStencilAttachment = nullptr;

        renderPassDesc.timestampWriteCount = 0;
        renderPassDesc.timestampWrites = nullptr;

        m_RenderPassEncoder = wgpuCommandEncoderBeginRenderPass(cmd.GetHandle(), &renderPassDesc);
    }

    void WebGPURendererAPI::EndMainRenderPass(BeeEngine::CommandBuffer commandBuffer)
    {
        wgpuRenderPassEncoderEnd(m_RenderPassEncoder);
    }

    void WebGPURendererAPI::SetClearColor(const BeeEngine::Color4 &color)
    {

    }

    void WebGPURendererAPI::Clear()
    {

    }

    void WebGPURendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {

    }

    void WebGPURendererAPI::DrawIndexed(const BeeEngine::Ref<BeeEngine::VertexArray> &vertexArray,
                                                             uint32_t indexCount)
    {

    }

    BeeEngine::Color4 WebGPURendererAPI::ReadPixel(uint32_t x, uint32_t y)
    {
        return BeeEngine::Color4();
    }

    RenderPass WebGPURendererAPI::GetMainRenderPass() const
    {
        return {m_RenderPassEncoder};
    }
}