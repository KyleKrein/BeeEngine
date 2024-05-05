//
// Created by Александр Лебедев on 30.06.2023.
//
#if defined(BEE_COMPILE_WEBGPU)
#include "WebGPUSwapchain.h"
#include "Core/Application.h"
#include "WebGPUGraphicsDevice.h"
#include "Windowing/WindowHandler/WindowHandler.h"

namespace BeeEngine::Internal
{

    WebGPUSwapChain::WebGPUSwapChain(WebGPUGraphicsDevice& device)
    {
        WGPUSwapChainDescriptor swapChainDesc = {};
        swapChainDesc.nextInChain = nullptr;

        swapChainDesc.width = WindowHandler::GetInstance()->GetWidthInPixels();
        swapChainDesc.height = WindowHandler::GetInstance()->GetHeightInPixels();
        BeeCoreTrace("Creating swapchain with size: {}x{}", swapChainDesc.width, swapChainDesc.height);
        m_Width = swapChainDesc.width;
        m_Height = swapChainDesc.height;

        m_Format = WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm; // wgpuSurfaceGetPreferredFormat(device.GetSurface(),
                                                                    // device.GetAdapter());
        swapChainDesc.format = m_Format;

        swapChainDesc.usage = WGPUTextureUsage_RenderAttachment;
        if (WindowHandler::GetInstance()->GetVSync() == VSync::On)
            swapChainDesc.presentMode = WGPUPresentMode_Fifo;
        else if constexpr (Application::GetOsPlatform() == OSPlatform::Mac)
            swapChainDesc.presentMode = WGPUPresentMode_Immediate;
        else
            swapChainDesc.presentMode = WGPUPresentMode_Mailbox;

        m_SwapChain = wgpuDeviceCreateSwapChain(device.GetDevice(), device.GetSurface(), &swapChainDesc);
        m_DepthFormat = WGPUTextureFormat_Depth24Plus; // TODO: make it not hardcoded
        WGPUTextureDescriptor depthTextureDesc;
        depthTextureDesc.nextInChain = nullptr;
        depthTextureDesc.label = "Depth Texture";
        depthTextureDesc.dimension = WGPUTextureDimension_2D;
        depthTextureDesc.format = m_DepthFormat;
        depthTextureDesc.mipLevelCount = 1;
        depthTextureDesc.sampleCount = 1;
        depthTextureDesc.size = {m_Width, m_Height, 1};
        depthTextureDesc.usage = WGPUTextureUsage_RenderAttachment;
        depthTextureDesc.viewFormatCount = 1;
        depthTextureDesc.viewFormats = &m_DepthFormat;
        m_DepthTexture = wgpuDeviceCreateTexture(device.GetDevice(), &depthTextureDesc);

        // Create the view of the depth texture manipulated by the rasterizer
        WGPUTextureViewDescriptor depthTextureViewDesc;
        depthTextureViewDesc.nextInChain = nullptr;
        depthTextureViewDesc.label = "Depth Texture View";
        depthTextureViewDesc.aspect = WGPUTextureAspect_DepthOnly;
        depthTextureViewDesc.baseArrayLayer = 0;
        depthTextureViewDesc.arrayLayerCount = 1;
        depthTextureViewDesc.baseMipLevel = 0;
        depthTextureViewDesc.mipLevelCount = 1;
        depthTextureViewDesc.dimension = WGPUTextureViewDimension_2D;
        depthTextureViewDesc.format = m_DepthFormat;
        m_DepthTextureView = wgpuTextureCreateView(m_DepthTexture, &depthTextureViewDesc);
    }

    WebGPUSwapChain::~WebGPUSwapChain()
    {
        wgpuTextureViewRelease(m_DepthTextureView);
        wgpuTextureDestroy(m_DepthTexture);
        wgpuTextureRelease(m_DepthTexture);
        wgpuSwapChainRelease(m_SwapChain);
    }
} // namespace BeeEngine::Internal
#endif