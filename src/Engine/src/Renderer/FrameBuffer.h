//
// Created by alexl on 14.05.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include "Core/Color4.h"

namespace BeeEngine
{
    enum class FrameBufferTextureFormat
    {
        None = 0,
        //color
        RGBA8,
        RedInteger,

        //depth/stencil
        Depth24,

        //defaults
        Depth = Depth24
    };
    enum class FrameBufferTextureUsage
    {
        None = 0,
        GPUOnly = 1,
        CPUAndGPU = 2
    };

    struct FrameBufferTextureSpecification
    {
        FrameBufferTextureSpecification() = default;

        FrameBufferTextureSpecification(FrameBufferTextureFormat format)
                : TextureFormat(format)
        {}
        //FrameBufferTextureSpecification(FrameBufferTextureFormat format, FrameBufferTextureUsage usage)
        //        : TextureFormat(format), TextureUsage(usage)
        //{}

        FrameBufferTextureFormat TextureFormat = FrameBufferTextureFormat::None;
        Color4 ClearColor = Color4::CornflowerBlue;
        float ClearDepth = 1.0f;
        int32_t ClearRedInteger= -1;
        //FrameBufferTextureUsage TextureUsage = FrameBufferTextureUsage::GPUOnly;
    };

    struct FrameBufferAttachmentSpecification
    {
        FrameBufferAttachmentSpecification() = default;
        FrameBufferAttachmentSpecification(std::initializer_list<FrameBufferTextureSpecification> attachments)
                : Attachments(attachments)
        {}

        std::vector<FrameBufferTextureSpecification> Attachments;
    };

    struct FrameBufferPreferences
    {
        uint32_t Width;
        uint32_t Height;
        uint32_t Samples = 1;
        bool SwapChainTarget = false;
        FrameBufferAttachmentSpecification Attachments;

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
        FrameBuffer() = default;
        virtual ~FrameBuffer() = default;
        virtual void Bind() = 0;
        virtual void Unbind() const = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual void Invalidate() = 0;
        [[nodiscard]] virtual uintptr_t GetColorAttachmentRendererID(uint32_t index) const = 0;
        [[nodiscard]] virtual uintptr_t GetDepthAttachmentRendererID() const = 0;
        [[nodiscard]] virtual uint32_t GetRendererID() const = 0;
        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) const = 0;
        virtual void ClearColorAttachment(uint32_t attachmentIndex, int value) = 0;
        static Scope<FrameBuffer> Create(const FrameBufferPreferences& preferences);
        static Scope<FrameBuffer> Create(FrameBufferPreferences&& preferences);

    protected:
        bool IsDepthFormat(FrameBufferTextureFormat format) const;
    };
}
