//
// Created by alexl on 14.05.2023.
//

#pragma once
#include "Core/TypeDefines.h"

namespace BeeEngine
{
    struct FrameBufferPreferences
    {
        uint32_t Width;
        uint32_t Height;
        uint32_t Samples = 1;
        bool SwapChainTarget = false;

        FrameBufferPreferences()
                : Width(1200), Height(720), Samples(1), SwapChainTarget(false)
        {}
        FrameBufferPreferences(uint32_t width, uint32_t height, uint32_t samples = 1, bool swapChainTarget = false)
                : Width(width), Height(height), Samples(samples), SwapChainTarget(swapChainTarget)
        {}
    };
    class FrameBuffer
    {
    public:
        explicit FrameBuffer(const FrameBufferPreferences& preferences);
        virtual ~FrameBuffer() = default;
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual void Invalidate() = 0;
        [[nodiscard]] inline uint32_t GetColorAttachmentRendererID() const
        {
            return m_ColorAttachment;
        }
        [[nodiscard]] inline uint32_t GetDepthAttachmentRendererID() const
        {
            return m_DepthAttachment;
        }
        [[nodiscard]] inline uint32_t GetRendererID() const
        {
            return m_RendererID;
        }
        static Scope<FrameBuffer> Create(const FrameBufferPreferences& preferences);
        static Scope<FrameBuffer> Create(FrameBufferPreferences&& preferences);

    protected:
        uint32_t m_RendererID;
        uint32_t m_ColorAttachment, m_DepthAttachment;
        FrameBufferPreferences m_Preferences;
    };
}
