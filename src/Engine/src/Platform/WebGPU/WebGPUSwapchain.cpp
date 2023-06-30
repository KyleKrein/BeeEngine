//
// Created by Александр Лебедев on 30.06.2023.
//

#include "WebGPUSwapchain.h"
#include "Windowing/WindowHandler/WindowHandler.h"
#include "WebGPUGraphicsDevice.h"

namespace BeeEngine::Internal
{

    WebGPUSwapChain::WebGPUSwapChain(WebGPUGraphicsDevice& device)
    {
        WGPUSwapChainDescriptor swapChainDesc = {};
        swapChainDesc.nextInChain = nullptr;

        swapChainDesc.width = WindowHandler::GetInstance()->GetWidth();
        swapChainDesc.height = WindowHandler::GetInstance()->GetHeight();

        WGPUTextureFormat swapChainFormat = WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm;//wgpuSurfaceGetPreferredFormat(device.GetSurface(), device.GetAdapter());
        swapChainDesc.format = swapChainFormat;

        swapChainDesc.usage = WGPUTextureUsage_RenderAttachment;
        if(WindowHandler::GetInstance()->GetVSync() == VSync::On)
            swapChainDesc.presentMode = WGPUPresentMode_Fifo;
        else
            swapChainDesc.presentMode = WGPUPresentMode_Mailbox;

        m_SwapChain = wgpuDeviceCreateSwapChain(device.GetDevice(), device.GetSurface(), &swapChainDesc);
    }

    WebGPUSwapChain::~WebGPUSwapChain()
    {
        wgpuSwapChainRelease(m_SwapChain);
    }
}