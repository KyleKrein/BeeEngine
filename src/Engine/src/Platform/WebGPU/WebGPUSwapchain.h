//
// Created by Александр Лебедев on 30.06.2023.
//

#pragma once

#include <webgpu/webgpu.h>

namespace BeeEngine::Internal
{
    class WebGPUSwapChain
    {
    public:
        WebGPUSwapChain(class WebGPUGraphicsDevice& device);
        ~WebGPUSwapChain();
        [[nodiscard]] WGPUSwapChain GetHandle() const { return m_SwapChain; }
    private:
        WGPUSwapChain m_SwapChain;
    };
}