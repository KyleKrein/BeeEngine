//
// Created by Александр Лебедев on 30.06.2023.
//

#pragma once

#include "Renderer/GraphicsDevice.h"
#include "WebGPUInstance.h"
#include "WebGPUSwapchain.h"
#include "WebGPUCommandBuffer.h"
#include "gsl/gsl"

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

        WGPUBuffer CreateBuffer(WGPUBufferUsageFlags usage, uint32_t size);
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

    private:
        static WebGPUGraphicsDevice* s_Instance;
        static WGPUAdapter RequestAdapter(WGPUInstance instance, WGPURequestAdapterOptions const * options);
        WGPUSurface CreateSurface(WGPUInstance instance);
        static WGPUDevice RequestDevice(WGPUAdapter adapter, WGPUDeviceDescriptor const * descriptor);
        WGPUQueue m_Queue;
        WGPUAdapter m_Adapter;
        WGPUSurface m_Surface;
        WGPUDevice m_Device;
        Scope<WebGPUSwapChain> m_SwapChain;

        bool m_SwapChainRequiresRebuild = false;

        void LogAdapterInfo() const noexcept;
    };
}
