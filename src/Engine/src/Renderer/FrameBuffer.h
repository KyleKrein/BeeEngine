//
// Created by alexl on 14.05.2023.
//

#pragma once
#include "Core/Color4.h"
#include "Core/Coroutines/Task.h"
#include "Core/TypeDefines.h"

namespace BeeEngine
{
    class CommandBuffer;
}

namespace BeeEngine
{
    enum class FrameBufferTextureFormat
    {
        None = 0,
        // color
        RGBA8,
        RGBA16F,
        RedInteger,

        // depth/stencil
        Depth24,

        // defaults
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

        FrameBufferTextureSpecification(FrameBufferTextureFormat format) : TextureFormat(format) {}
        // FrameBufferTextureSpecification(FrameBufferTextureFormat format, FrameBufferTextureUsage usage)
        //         : TextureFormat(format), TextureUsage(usage)
        //{}

        FrameBufferTextureFormat TextureFormat = FrameBufferTextureFormat::None;
        Color4 ClearColor = Color4::CornflowerBlue;
        float ClearDepth = 1.0f;
        int32_t ClearRedInteger = -1;
        FrameBufferTextureUsage TextureUsage = FrameBufferTextureUsage::GPUOnly;
    };

    struct FrameBufferAttachmentSpecification
    {
        FrameBufferAttachmentSpecification() = default;
        FrameBufferAttachmentSpecification(std::initializer_list<FrameBufferTextureSpecification> attachments)
            : Attachments(attachments)
        {
        }

        std::vector<FrameBufferTextureSpecification> Attachments;
    };

    struct FrameBufferPreferences
    {
        uint32_t Width;
        uint32_t Height;
        uint32_t Samples = 1;
        bool SwapChainTarget = false;
        FrameBufferAttachmentSpecification Attachments;

        FrameBufferPreferences() : Width(1200), Height(720), Samples(1), SwapChainTarget(false) {}
        FrameBufferPreferences(uint32_t width, uint32_t height, uint32_t samples = 1, bool swapChainTarget = false)
            : Width(width), Height(height), Samples(samples), SwapChainTarget(swapChainTarget)
        {
        }
    };
    class DumpedImage final
    {
    public:
        DumpedImage(void* data, uint32_t width, uint32_t height, uint32_t sizeOfPixel, FrameBufferTextureFormat format)
            : m_Width(width), m_Height(height), m_SizeOfPixel(sizeOfPixel), m_Format(format)
        {
            m_Data = malloc(width * height * sizeOfPixel);
            memcpy(m_Data, data, width * height * sizeOfPixel);
        }
        DumpedImage(const DumpedImage& other) = delete;
        DumpedImage& operator=(const DumpedImage& other) = delete;
        DumpedImage(DumpedImage&& other) noexcept
        {
            m_Data = other.m_Data;
            m_Width = other.m_Width;
            m_Height = other.m_Height;
            m_SizeOfPixel = other.m_SizeOfPixel;
            m_Format = other.m_Format;
            other.m_Data = nullptr;
        }
        DumpedImage& operator=(DumpedImage&& other) noexcept
        {
            m_Data = other.m_Data;
            m_Width = other.m_Width;
            m_Height = other.m_Height;
            m_SizeOfPixel = other.m_SizeOfPixel;
            m_Format = other.m_Format;
            other.m_Data = nullptr;
            return *this;
        }
        ~DumpedImage()
        {
            if (m_Data)
            {
                free(m_Data);
            }
        }
        [[nodiscard]] void* GetData() const { return m_Data; }
        [[nodiscard]] uint32_t GetWidth() const { return m_Width; }
        [[nodiscard]] uint32_t GetHeight() const { return m_Height; }
        [[nodiscard]] uint32_t GetSizeOfPixel() const { return m_SizeOfPixel; }
        [[nodiscard]] FrameBufferTextureFormat GetFormat() const { return m_Format; }

    private:
        void* m_Data = nullptr;
        uint32_t m_Width;
        uint32_t m_Height;
        uint32_t m_SizeOfPixel;
        FrameBufferTextureFormat m_Format;
    };
    class FrameBuffer
    {
    public:
        FrameBuffer() = default;
        virtual ~FrameBuffer() = default;
        [[nodiscard]] virtual CommandBuffer Bind() = 0;
        virtual void Unbind(CommandBuffer& commandBuffer) = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;
        virtual void Invalidate() = 0;
        [[nodiscard]] virtual uintptr_t GetColorAttachmentRendererID(uint32_t index) const = 0;
        [[nodiscard]] virtual uintptr_t GetDepthAttachmentRendererID() const = 0;
        [[nodiscard]] virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) const = 0;
        [[nodiscard]] virtual DumpedImage DumpAttachment(uint32_t attachmentIndex) const = 0;
        static Scope<FrameBuffer> Create(const FrameBufferPreferences& preferences);
        static Scope<FrameBuffer> Create(FrameBufferPreferences&& preferences);

    protected:
        bool IsDepthFormat(FrameBufferTextureFormat format) const;
    };
} // namespace BeeEngine
