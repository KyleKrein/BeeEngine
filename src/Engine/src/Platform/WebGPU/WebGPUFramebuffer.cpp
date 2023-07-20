//
// Created by Александр Лебедев on 18.07.2023.
//

#include "WebGPUFramebuffer.h"
#include "Debug/Instrumentor.h"
#include "WebGPUGraphicsDevice.h"
#include "Renderer/Renderer.h"

namespace BeeEngine::Internal
{
    WGPUTexture WebGPUFrameBuffer::CreateTexture(FrameBufferTextureFormat format)
    {
        auto wgpuformat = ConvertToWebGPUTextureFormat(format);

        WGPUTextureDescriptor descriptor{};
        descriptor.nextInChain = nullptr;
        descriptor.label = "Framebuffer Texture";
        descriptor.size = {m_Preferences.Width, m_Preferences.Height, 1};
        descriptor.format = wgpuformat;
        descriptor.usage = IsDepthFormat(format)? WGPUTextureUsage_CopySrc | WGPUTextureUsage_RenderAttachment : WGPUTextureUsage_CopySrc | WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment;
        descriptor.dimension = WGPUTextureDimension_2D;
        descriptor.sampleCount = m_Preferences.Samples;
        descriptor.mipLevelCount = 1;
        descriptor.viewFormats = &wgpuformat;
        descriptor.viewFormatCount = 1;
        return wgpuDeviceCreateTexture(m_GraphicsDevice.GetDevice(), &descriptor);
    }

    WGPUTextureView WebGPUFrameBuffer::CreateTextureView(WGPUTexture texture, FrameBufferTextureFormat format)
    {
        WGPUTextureViewDescriptor descriptor{};
        descriptor.nextInChain = nullptr;
        descriptor.label = "Framebuffer Texture View";
        descriptor.mipLevelCount = 1;
        descriptor.dimension = WGPUTextureViewDimension_2D;
        descriptor.aspect = IsDepthFormat(format) ? WGPUTextureAspect_DepthOnly : WGPUTextureAspect_All;
        descriptor.format = ConvertToWebGPUTextureFormat(format);
        descriptor.arrayLayerCount = 1;
        descriptor.baseArrayLayer = 0;
        descriptor.baseMipLevel = 0;

        return wgpuTextureCreateView(texture, &descriptor);
    }


    WebGPUFrameBuffer::WebGPUFrameBuffer(const FrameBufferPreferences &preferences)
            : m_Preferences(preferences), m_GraphicsDevice(WebGPUGraphicsDevice::GetInstance())
    {
        BEE_PROFILE_FUNCTION();
        for (auto specification: m_Preferences.Attachments.Attachments)
        {
            if(!IsDepthFormat(specification.TextureFormat))
            {
                m_ColorAttachmentSpecification.emplace_back(specification);
            }
            else
            {
                m_DepthAttachmentSpecification = specification;
            }
        }
        m_CurrentCommandBuffer = m_GraphicsDevice.CreateCommandBuffer();
        WebGPUFrameBuffer::Invalidate();
        m_Initiated = true;
    }

    void WebGPUFrameBuffer::Resize(uint32_t width, uint32_t height)
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(width > 0 && height > 0 && width < 100000 && height < 100000);
        m_Preferences.Width = width;
        m_Preferences.Height = height;
        m_Invalid = true;
        // Setup viewport
        //if(m_CurrentRenderPass.GetHandle())
            //wgpuRenderPassEncoderSetViewport((WGPURenderPassEncoder)m_CurrentRenderPass.GetHandle(), 0, 0, m_Preferences.Width, m_Preferences.Height, 0, 1);
    }

    WebGPUFrameBuffer::~WebGPUFrameBuffer()
    {
        BEE_PROFILE_FUNCTION();
        if(m_Initiated)
        {
            m_GraphicsDevice.DestroyTexture(m_DepthAttachmentTexture);
            m_GraphicsDevice.DestroyTextureView(m_DepthAttachmentTextureView);
            for(auto& textureView: m_ColorAttachmentsTextureViews)
            {
                m_GraphicsDevice.DestroyTextureView(textureView);
            }
            for(auto& texture: m_ColorAttachmentsTextures)
            {
                m_GraphicsDevice.DestroyTexture(texture);
            }
            if(m_EntityIDBuffer)
            {
                wgpuBufferDestroy(m_EntityIDBuffer);
                wgpuBufferRelease(m_EntityIDBuffer);
            }
        }
    }

    void WebGPUFrameBuffer::Bind()
    {
        BEE_PROFILE_FUNCTION();
        Renderer::Flush();
        if(m_Invalid)
        {
            Invalidate();
        }
        m_CurrentCommandBuffer = m_GraphicsDevice.CreateCommandBuffer();

        std::vector<WGPURenderPassColorAttachment> colorAttachments{};
        const auto size = m_ColorAttachmentSpecification.size();

        colorAttachments.reserve(size);


        for(size_t i = 0; i < size; ++i)
        {
            WGPURenderPassColorAttachment colorAttachment{};
            colorAttachment.nextInChain = nullptr;
            colorAttachment.resolveTarget = nullptr;
            colorAttachment.loadOp = WGPULoadOp_Clear;
            colorAttachment.storeOp = WGPUStoreOp_Store;
            colorAttachment.view = m_ColorAttachmentsTextureViews[i];
            colorAttachment.clearValue = m_ColorAttachmentSpecification[i].ClearColor;

            colorAttachments.push_back(colorAttachment);
        }

        WGPURenderPassDepthStencilAttachment depthStencilAttachment{};
        depthStencilAttachment.view = m_DepthAttachmentTextureView;
        depthStencilAttachment.depthClearValue = m_DepthAttachmentSpecification.ClearDepth;
        depthStencilAttachment.stencilClearValue = 0;
        depthStencilAttachment.depthLoadOp = WGPULoadOp_Clear;
        depthStencilAttachment.depthStoreOp = WGPUStoreOp_Store;
        depthStencilAttachment.stencilLoadOp = WGPULoadOp_Undefined;
        depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Undefined;
        depthStencilAttachment.depthReadOnly = false;
        depthStencilAttachment.stencilReadOnly = true;
        
        WGPURenderPassDescriptor descriptor{};
        descriptor.nextInChain = nullptr;
        descriptor.label = "Framebuffer Render Pass";
        descriptor.colorAttachments = colorAttachments.data();
        descriptor.colorAttachmentCount = colorAttachments.size();
        if(m_DepthAttachmentTexture)
        {
            descriptor.depthStencilAttachment = &depthStencilAttachment;
        }
        else
        {
            descriptor.depthStencilAttachment = nullptr;
        }
        descriptor.occlusionQuerySet = nullptr;
        descriptor.timestampWriteCount = 0;
        descriptor.timestampWrites = nullptr;

        auto renderPass = wgpuCommandEncoderBeginRenderPass(((WebGPUCommandBuffer*)&m_CurrentCommandBuffer)->GetHandle(), &descriptor);
        m_CurrentRenderPass = {renderPass};
        Renderer::SetCurrentRenderPass(m_CurrentRenderPass);
        //wgpuRenderPassEncoderSetViewport(renderPass, 0, 0, m_Preferences.Width, m_Preferences.Height, 0, 1);
    }

    void WebGPUFrameBuffer::Unbind()
    {
        BEE_PROFILE_FUNCTION();
        //wgpuRenderPassEncoderSetViewport((WGPURenderPassEncoder)m_CurrentRenderPass.GetHandle(), 0, 0, m_Preferences.Width, m_Preferences.Height, 0, 1);
        // Apply scissor/clipping rectangle, Draw
        //wgpuRenderPassEncoderSetScissorRect((WGPURenderPassEncoder)m_CurrentRenderPass.GetHandle(), 0, 0, m_Preferences.Width, m_Preferences.Height);
        Renderer::Flush();
        wgpuRenderPassEncoderEnd((WGPURenderPassEncoder)m_CurrentRenderPass.GetHandle());
        Renderer::SubmitCommandBuffer(m_CurrentCommandBuffer);
        //m_GraphicsDevice.SubmitCommandBuffers(&m_CurrentCommandBuffer, 1);
        Renderer::ResetCurrentRenderPass();
        m_CurrentRenderPass = {nullptr};
    }

    void WebGPUFrameBuffer::Invalidate()
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(m_Preferences.Width > 0 && m_Preferences.Height > 0 && m_Preferences.Width < 100000 && m_Preferences.Height < 100000);
        if(m_Initiated)
        {
            for (size_t i = 0; i < m_ColorAttachmentsTextures.size(); ++i)
            {
                m_GraphicsDevice.DestroyTextureView(m_ColorAttachmentsTextureViews[i]);
                m_GraphicsDevice.DestroyTexture(m_ColorAttachmentsTextures[i]);
            }
            if(m_DepthAttachmentTexture)
            {
                m_GraphicsDevice.DestroyTextureView(m_DepthAttachmentTextureView);
                m_GraphicsDevice.DestroyTexture(m_DepthAttachmentTexture);
            }
            m_ColorAttachmentsTextures.clear();
            m_ColorAttachmentsTextureViews.clear();
            m_DepthAttachmentTexture = nullptr;
            m_DepthAttachmentTextureView = nullptr;
        }
        m_ColorAttachmentsTextures.resize(m_ColorAttachmentSpecification.size());
        m_ColorAttachmentsTextureViews.resize(m_ColorAttachmentSpecification.size());
        for (size_t i = 0; i < m_ColorAttachmentSpecification.size(); ++i)
        {
            WGPUTexture texture = CreateTexture(m_ColorAttachmentSpecification[i].TextureFormat);
            m_ColorAttachmentsTextures[i] = texture;
            m_ColorAttachmentsTextureViews[i] = CreateTextureView(texture, m_ColorAttachmentSpecification[i].TextureFormat);
        }
        if(m_DepthAttachmentSpecification.TextureFormat != FrameBufferTextureFormat::None)
        {
            WGPUTexture depthTexture = CreateTexture(m_DepthAttachmentSpecification.TextureFormat);
            m_DepthAttachmentTexture = depthTexture;
            m_DepthAttachmentTextureView = CreateTextureView(depthTexture, m_DepthAttachmentSpecification.TextureFormat);
        }
        m_Invalid = false;
    }

    uint32_t WebGPUFrameBuffer::GetRendererID() const
    {
        BeeExpects(false);
        return -1;
    }
    /*
    struct ReadPixelData
    {
        CommandBuffer CommandBuffer;
        WGPUBuffer Buffer;

    };
    void WebGPUFrameBuffer::ReadPixelsAsync(uint32_t attachmentIndex, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const std::function<void(std::vector<uint8_t> data)>& callback)
    {
        BeeExpects(attachmentIndex < m_ColorAttachmentsTextureViews.size());

    }
*/
    int WebGPUFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y) const
    {
        return -1;
        BeeExpects(attachmentIndex < m_ColorAttachmentsTextureViews.size());
        if(m_WaitingForReadPixel)
            return static_cast<int>(m_ReadPixelValue);
        if(!m_EntityIDBuffer)
            m_EntityIDBuffer = m_GraphicsDevice.CreateBuffer(WGPUBufferUsage_MapRead | WGPUBufferUsage_CopyDst, sizeof (int));
        if(!m_BufferCopyEncoder)
            m_BufferCopyEncoder = m_GraphicsDevice.CreateCommandBuffer();
        auto texture = m_ColorAttachmentsTextures[attachmentIndex];
        WGPUImageCopyTexture textureCopy{};
        textureCopy.nextInChain = nullptr;
        textureCopy.mipLevel = 0;
        textureCopy.origin = {static_cast<uint32_t>(x), static_cast<uint32_t>(y), 0};
        textureCopy.aspect = WGPUTextureAspect_All;
        textureCopy.texture = texture;

        WGPUImageCopyBuffer bufferCopy{};
        bufferCopy.nextInChain = nullptr;
        bufferCopy.buffer = m_EntityIDBuffer;
        bufferCopy.layout.nextInChain = nullptr;
        bufferCopy.layout.offset = 0;
        bufferCopy.layout.bytesPerRow = std::max(m_Preferences.Width, m_Preferences.Height) * sizeof(int) + 256 - (std::max(m_Preferences.Width, m_Preferences.Height) * sizeof(int) % 256);
        bufferCopy.layout.rowsPerImage = 0;

        WGPUExtent3D extent{0,0,0};
        wgpuCommandEncoderCopyTextureToBuffer(((WebGPUCommandBuffer*)&m_BufferCopyEncoder)->GetHandle(), &textureCopy, &bufferCopy, &extent);
        Renderer::SubmitCommandBuffer(m_BufferCopyEncoder);
        m_WaitingForReadPixel = true;

        DeletionQueue::Frame().PushFunction([this]()
        {
            auto OnBufferMapped = [](WGPUBufferMapAsyncStatus status, void* userdata)
            {
                BufferContext* context = (BufferContext*)userdata;
                if(status != WGPUBufferMapAsyncStatus_Success)
                {
                    BeeCoreWarn("Failed to map buffer: {}", ToString(status));
                    return ;
                }
                float* bufferData = (float*)wgpuBufferGetMappedRange(*context->Buffer, 0, sizeof(int));
                if(bufferData == nullptr)
                {
                    BeeCoreWarn("Failed to map buffer");
                    return ;
                }
                memcpy(context->Data, bufferData, sizeof(int));

                wgpuBufferUnmap(*context->Buffer);
            };

            wgpuBufferMapAsync(m_EntityIDBuffer, WGPUMapMode_Read, 0, sizeof(int), OnBufferMapped, &m_EntityIDBufferContext);
            m_WaitingForReadPixel = false;
        });

        m_BufferCopyEncoder = nullptr;
        return static_cast<int>(m_ReadPixelValue);
    }

    void WebGPUFrameBuffer::ClearColorAttachment(uint32_t attachmentIndex, int value)
    {

    }

    WGPUTextureFormat WebGPUFrameBuffer::ConvertToWebGPUTextureFormat(FrameBufferTextureFormat format)
    {
        switch (format)
        {
            case FrameBufferTextureFormat::None:
                return WGPUTextureFormat_Undefined;
            case FrameBufferTextureFormat::RGBA8:
                return WGPUTextureFormat_BGRA8Unorm;
            case FrameBufferTextureFormat::RedInteger:
                return WGPUTextureFormat_R32Float;
            case FrameBufferTextureFormat::Depth24:
                return WGPUTextureFormat_Depth24Plus;
        }
        return WGPUTextureFormat_Undefined;
    }

    bool WebGPUFrameBuffer::IsDepthFormat(FrameBufferTextureFormat format)
    {
        switch (format)
        {
            case FrameBufferTextureFormat::Depth24:
                return true;
            default:
                return false;
        }
    }
}