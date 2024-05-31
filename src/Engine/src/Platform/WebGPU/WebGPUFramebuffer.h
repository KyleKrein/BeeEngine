//
// Created by Александр Лебедев on 18.07.2023.
//

#pragma once
#if defined(BEE_COMPILE_WEBGPU)
#include "Renderer/CommandBuffer.h"
#include "Renderer/FrameBuffer.h"
#include "Renderer/RenderPass.h"

namespace BeeEngine::Internal
{
    class WebGPUFrameBuffer : public FrameBuffer
    {
    public:
        WebGPUFrameBuffer(const FrameBufferPreferences& preferences);
        ~WebGPUFrameBuffer() override;
        void Bind() override;
        void Unbind() override;
        void Flush(const std::function<void()>& callback) override;
        void Resize(uint32_t width, uint32_t height) override;
        void Invalidate() override;
        [[nodiscard]] uintptr_t GetColorAttachmentRendererID(uint32_t index) const override
        {
            BeeExpects(index < m_ColorAttachmentsTextureViews.size());
            auto textureView = m_ColorAttachmentsTextureViews[index];
            // wgpuTextureViewReference(textureView);
            return (uintptr_t)textureView;
        }
        [[nodiscard]] uintptr_t GetDepthAttachmentRendererID() const override
        {
            // wgpuTextureViewReference(m_DepthAttachmentTextureView);
            return (uintptr_t)m_DepthAttachmentTextureView;
        }
        int ReadPixel(uint32_t attachmentIndex, int x, int y) const override;

    private:
        std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecification;
        FrameBufferTextureSpecification m_DepthAttachmentSpecification;
        FrameBufferPreferences m_Preferences;
        bool m_Initiated{false};

        std::vector<WGPUTexture> m_ColorAttachmentsTextures;
        std::vector<WGPUTextureView> m_ColorAttachmentsTextureViews;
        WGPUTexture m_DepthAttachmentTexture{nullptr};
        WGPUTextureView m_DepthAttachmentTextureView{nullptr};

        bool m_Invalid{true};

        mutable CommandBuffer m_CurrentCommandBuffer;
        mutable RenderPass m_CurrentRenderPass;
        class WebGPUGraphicsDevice& m_GraphicsDevice;

        WGPUTexture CreateTexture(FrameBufferTextureFormat format);
        WGPUTextureView CreateTextureView(WGPUTexture texture, FrameBufferTextureFormat format);

        static WGPUTextureFormat ConvertToWebGPUTextureFormat(FrameBufferTextureFormat format);
        static bool IsDepthFormat(FrameBufferTextureFormat format);

        mutable float m_ReadPixelValue{-1.0f};
        mutable WGPUBuffer m_EntityIDBuffer{nullptr};

        struct BufferContext
        {
            void* Data;
            WGPUBuffer* Buffer;
            bool* Waiting;
        };
        mutable CommandBuffer m_BufferCopyEncoder{nullptr};

        mutable bool m_WaitingForReadPixel{false};
        mutable BufferContext m_EntityIDBufferContext{&m_ReadPixelValue, &m_EntityIDBuffer, &m_WaitingForReadPixel};

        std::function<void()> m_FlushCallback;
        bool m_Flushed{true};
        struct FlushCallbackData
        {
            std::function<void()>* Callback;
            bool* Flushed;
        };
        FlushCallbackData m_FlushCallbackData{&m_FlushCallback, &m_Flushed};
    };
} // namespace BeeEngine::Internal
#endif