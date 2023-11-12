//
// Created by Александр Лебедев on 30.06.2023.
//

#pragma once

#include "Renderer/GraphicsDevice.h"
#include "WebGPUInstance.h"
#include "WebGPUSwapchain.h"
#include "WebGPUCommandBuffer.h"
#include "gsl/gsl"
#include "WebGPUBufferPool.h"
#include "Core/Coroutines/Task.h"

namespace BeeEngine::Internal
{
    class WebGPUGraphicsDevice: public GraphicsDevice
    {
    public:
        WebGPUGraphicsDevice(WebGPUInstance& instance);
        ~WebGPUGraphicsDevice() override;

        [[nodiscard]] WGPUAdapter GetAdapter() const
        {
            return m_Adapter;
        }

        [[nodiscard]] WGPUSurface GetSurface() const
        {
            return m_Surface;
        }

        [[nodiscard]] WGPUDevice GetDevice() const
        {
            return m_Device;
        }

        static WebGPUGraphicsDevice& GetInstance()
        {
            return *s_Instance;
        }

        WebGPUSwapChain& GetSwapChain()
        {
            return *m_SwapChain;
        }
        WGPUQueue GetQueue() const
        {
            return m_Queue;
        }

        WGPUBuffer CreateBuffer(WGPUBufferUsageFlags usage, uint32_t size);
        [[nodiscard("To avoid gpu memory leaks. Don't forget to call ReleaseBuffer()")]]
        const WebGPUBuffer& RequestBuffer(uint32_t size, WebGPUBufferUsage usage)
        {
            return m_BufferPool->RequestBuffer(size, usage);
        }
        void ReleaseBuffer(const WebGPUBuffer& buffer)
        {
            m_BufferPool->ReleaseBuffer(buffer);
        }
        void CopyDataToBuffer(gsl::span<byte> data, WGPUBuffer buffer);

        void SubmitCommandBuffers(CommandBuffer* commandBuffers, uint32_t numberOfBuffers);

        CommandBuffer CreateCommandBuffer();
        void WindowResized(uint32_t width, uint32_t height) override;

        void RequestSwapChainRebuild() override
        {
            m_SwapChainRequiresRebuild = true;
        }
        bool SwapChainRequiresRebuild() override
        {
            return m_SwapChainRequiresRebuild;
        }

        void SetDefault(out<WGPULimits> limits) const noexcept
        {
            memset(&limits, 0, sizeof(WGPULimits));
        }
        void SetDefault(out<WGPUBindGroupEntry> entry) const noexcept
        {
            entry.nextInChain = nullptr;
            entry.binding = 0;
            entry.buffer = nullptr;
            entry.offset = 0;
            entry.size = 0;
            entry.sampler = nullptr;
            entry.textureView = nullptr;
        }

        void SetDefault(out<WGPUBindGroupLayoutEntry> bindingLayout) const noexcept
        {
            bindingLayout.buffer.nextInChain = nullptr;
            bindingLayout.buffer.type = WGPUBufferBindingType_Undefined;
            bindingLayout.buffer.hasDynamicOffset = false;

            bindingLayout.sampler.nextInChain = nullptr;
            bindingLayout.sampler.type = WGPUSamplerBindingType_Undefined;

            bindingLayout.storageTexture.nextInChain = nullptr;
            bindingLayout.storageTexture.access = WGPUStorageTextureAccess_Undefined;
            bindingLayout.storageTexture.format = WGPUTextureFormat_Undefined;
            bindingLayout.storageTexture.viewDimension = WGPUTextureViewDimension_Undefined;

            bindingLayout.texture.nextInChain = nullptr;
            bindingLayout.texture.multisampled = false;
            bindingLayout.texture.sampleType = WGPUTextureSampleType_Undefined;
            bindingLayout.texture.viewDimension = WGPUTextureViewDimension_Undefined;
        }

        void DestroyTexture(WGPUTexture texture)
        {
            wgpuTextureDestroy(texture);
            wgpuTextureRelease(texture);
        }
        void DestroyTextureView(WGPUTextureView textureView)
        {
            wgpuTextureViewRelease(textureView);
        }

        const WGPUSupportedLimits& GetSupportedLimits() const noexcept
        {
            return m_SupportedLimits;
        }

        Task<> WaitForQueueIdle();

    private:
        static WebGPUGraphicsDevice* s_Instance;
        static WGPUAdapter RequestAdapter(WGPUInstance instance, WGPURequestAdapterOptions const * options);
        WGPUSurface CreateSurface(WGPUInstance instance);
        static WGPUDevice RequestDevice(WGPUAdapter adapter, WGPUDeviceDescriptor const * descriptor);
        WGPUQueue m_Queue;
        WGPUAdapter m_Adapter;
        WGPUSurface m_Surface;
        WGPUDevice m_Device;
        Scope<WebGPUBufferPool> m_BufferPool;
        Scope<WebGPUSwapChain> m_SwapChain;
        WGPUSupportedLimits m_SupportedLimits;

        bool m_SwapChainRequiresRebuild = false;

        void LogAdapterInfo() const noexcept;
    };
}
