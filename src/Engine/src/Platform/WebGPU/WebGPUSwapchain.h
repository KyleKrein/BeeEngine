//
// Created by Александр Лебедев on 30.06.2023.
//

#pragma once
#if defined(BEE_COMPILE_WEBGPU)
#include <webgpu/webgpu.h>

namespace BeeEngine::Internal
{
    class WebGPUSwapChain
    {
    public:
        WebGPUSwapChain(class WebGPUGraphicsDevice& device);
        ~WebGPUSwapChain();
        [[nodiscard]] WGPUSwapChain GetHandle() const { return m_SwapChain; }
        [[nodiscard]] uint32_t GetWidth() const { return m_Width; }
        [[nodiscard]] uint32_t GetHeight() const { return m_Height; }
        [[nodiscard]] WGPUTextureView GetDepthTextureView() const { return m_DepthTextureView; }
        [[nodiscard]] WGPUTextureFormat GetDepthFormat() const { return m_DepthFormat; }
        [[nodiscard]] WGPUTextureFormat GetFormat() const { return m_Format; }
    private:
        WGPUSwapChain m_SwapChain;
        WGPUTexture m_DepthTexture;
        WGPUTextureView m_DepthTextureView;
        WGPUTextureFormat m_DepthFormat;
        WGPUTextureFormat m_Format;
        uint32_t m_Width;
        uint32_t m_Height;
    };
}
#endif