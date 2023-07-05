//
// Created by Александр Лебедев on 30.06.2023.
//

#include "WebGPUGraphicsDevice.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_syswm.h"
#include "Windowing/WindowHandler/WindowHandler.h"
#include "Core/TypeDefines.h"

namespace BeeEngine::Internal
{
    WebGPUGraphicsDevice* WebGPUGraphicsDevice::s_Instance = nullptr;
    WebGPUGraphicsDevice::WebGPUGraphicsDevice(WebGPUInstance &instance)
    {
        BeeCoreAssert(s_Instance == nullptr, "WebGPUGraphicsDevice already created");
        s_Instance = this;
        m_Surface = CreateSurface(instance.GetHandle());
        WGPURequestAdapterOptions adapterOpts = {};
        adapterOpts.nextInChain = nullptr;
        adapterOpts.compatibleSurface = m_Surface;
        adapterOpts.powerPreference = WGPUPowerPreference::WGPUPowerPreference_HighPerformance;
        m_Adapter = RequestAdapter(instance.GetHandle(), &adapterOpts);

        std::vector<WGPUFeatureName> features;

        // Call the function a first time with a null return address, just to get
        // the entry count.
        size_t featureCount = wgpuAdapterEnumerateFeatures(m_Adapter, nullptr);

        // Allocate memory (could be a new, or a malloc() if this were a C program)
        features.resize(featureCount);

        // Call the function a second time, with a non-null return address
        wgpuAdapterEnumerateFeatures(m_Adapter, features.data());

        BeeCoreInfo("Adapter features:");
        for (auto f : features) {
            BeeCoreInfo(" - {}", ToString(f));
        }
#if defined(DEBUG)
        LogAdapterInfo();
#endif

        WGPUDeviceDescriptor deviceDescriptor = {};
        deviceDescriptor.nextInChain = nullptr;
        deviceDescriptor.label = "BeeEngine Device"; // anything works here, that's your call
        deviceDescriptor.requiredFeaturesCount = 0; // we do not require any specific feature
        deviceDescriptor.requiredLimits = nullptr; // we do not require any specific limit
        deviceDescriptor.defaultQueue.nextInChain = nullptr;
        deviceDescriptor.defaultQueue.label = "The default queue";

        m_Device = RequestDevice(m_Adapter, &deviceDescriptor);

        auto onDeviceError = [](WGPUErrorType type, char const* message, void* pUserData)
        {
            BeeCoreError("Uncaptured device error: type {0}, message: {1}", ToString(type), message? message : "null");
        };
        auto onDeviceLost = [](WGPUDeviceLostReason reason, char const* message, void* pUserData)
        {
            BeeCoreError("Device lost: reason {0}, message: {1}", ToString(reason), message? message : "null");
        };
        auto onDeviceLogging = [](WGPULoggingType type, char const* message, void* pUserData)
        {
            BeeCoreInfo("Device logging: type {0}, message: {1}", ToString(type), message? message : "null");
        };
        wgpuDeviceSetUncapturedErrorCallback(m_Device, onDeviceError, nullptr /* pUserData */);
        wgpuDeviceSetLoggingCallback(m_Device, onDeviceLogging, nullptr /* pUserData */);
        wgpuDeviceSetDeviceLostCallback(m_Device, onDeviceLost, nullptr /* pUserData */);

        m_Queue = wgpuDeviceGetQueue(m_Device);

        auto onQueueWorkDone = [](WGPUQueueWorkDoneStatus status, void* /* pUserData */) {
            BeeCoreTrace("Queued work finished with status: {}", ToString(status));
        };
        wgpuQueueOnSubmittedWorkDone(m_Queue, 0, onQueueWorkDone, nullptr /* pUserData */);//signal value ???

        m_SwapChain = CreateScope<WebGPUSwapChain>(*this);
    }

    WebGPUGraphicsDevice::~WebGPUGraphicsDevice()
    {
        m_SwapChain.reset();
        wgpuDeviceRelease(m_Device);
        wgpuAdapterRelease(m_Adapter);
        wgpuSurfaceRelease(m_Surface);
    }

    /**
 * Utility function to get a WebGPU adapter, so that
 *     WGPUAdapter adapter = requestAdapter(options);
 * is roughly equivalent to
 *     const adapter = await navigator.gpu.requestAdapter(options);
 */
    WGPUAdapter WebGPUGraphicsDevice::RequestAdapter(WGPUInstance instance, WGPURequestAdapterOptions const * options)
    {
        // A simple structure holding the local information shared with the
        // onAdapterRequestEnded callback.
        struct UserData {
            WGPUAdapter adapter = nullptr;
            bool requestEnded = false;
        };
        UserData userData;

        // Callback called by wgpuInstanceRequestAdapter when the request returns
        // This is a C++ lambda function, but could be any function defined in the
        // global scope. It must be non-capturing (the brackets [] are empty) so
        // that it behaves like a regular C function pointer, which is what
        // wgpuInstanceRequestAdapter expects (WebGPU being a C API). The workaround
        // is to convey what we want to capture through the pUserData pointer,
        // provided as the last argument of wgpuInstanceRequestAdapter and received
        // by the callback as its last argument.
        auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const * message, void * pUserData) {
            UserData& userData = *reinterpret_cast<UserData*>(pUserData);
            if (status == WGPURequestAdapterStatus_Success) {
                userData.adapter = adapter;
            } else {
                BeeCoreError("Could not get WebGPU adapter: {}", message);
            }
            userData.requestEnded = true;
        };

        // Call to the WebGPU request adapter procedure
        wgpuInstanceRequestAdapter(
                instance /* equivalent of navigator.gpu */,
                options,
                onAdapterRequestEnded,
                (void*)&userData
        );

        // In theory we should wait until onAdapterReady has been called, which
        // could take some time (what the 'await' keyword does in the JavaScript
        // code). In practice, we know that when the wgpuInstanceRequestAdapter()
        // function returns its callback has been called.
        BeeEnsures(userData.requestEnded);

        return userData.adapter;
    }

    WGPUSurface WebGPUGraphicsDevice::CreateSurface(WGPUInstance instance)
    {
        //Code taken from https://github.com/gecko0307/wgpu-dlang/blob/master/src/dgpu/core/gpu.d
        SDL_SysWMinfo wmInfo;
        SDL_GetWindowWMInfo((SDL_Window*)WindowHandler::GetInstance()->GetWindow(), &wmInfo, SDL_SYSWM_CURRENT_VERSION);

        WGPUSurface surface;
#if defined(WINDOWS)
        if (wmInfo.subsystem == SDL_SYSWM_WINDOWS)
        {
            auto win_hwnd = wmInfo.info.win.window;
            auto win_hinstance = wmInfo.info.win.hinstance;
            WGPUSurfaceDescriptorFromWindowsHWND sfdHwnd = {
                    chain: {
                            next: null,
                            sType: WGPUSType.SurfaceDescriptorFromWindowsHWND
                    },
                    hinstance: win_hinstance,
                    hwnd: win_hwnd
            };
            WGPUSurfaceDescriptor sfd = {
                    label: null,
                    nextInChain: cast(const(WGPUChainedStruct)*)&sfdHwnd
            };
            surface = wgpuInstanceCreateSurface(instance, &sfd);
        } else
        {
            BeeCoreError("Unsupported platform");
        }
#endif
#if defined(LINUX)
        // Needs test!
        if (wmInfo.subsystem == SDL_SYSWM_X11)
        {
            auto x11_display = wmInfo.info.x11.display;
            auto x11_window = wmInfo.info.x11.window;
            WGPUSurfaceDescriptorFromXlib sfdX11 = {
                    chain: {
                            next: null,
                            sType: WGPUSType.SurfaceDescriptorFromXlib
                    },
                    display: x11_display,
                    window: x11_window
            };
            WGPUSurfaceDescriptor sfd = {
                    label: null,
                    nextInChain: cast(const(WGPUChainedStruct)*)&sfdX11
            };
            surface = wgpuInstanceCreateSurface(instance, &sfd);
        }
        else
        {
            BeeCoreError("Unsupported subsystem, sorry");
        }
#endif
#if defined(MACOS)
        // Needs test!
        SDL_Renderer* renderer = SDL_CreateRenderer((SDL_Window*)WindowHandler::GetInstance()->GetWindow(), "metal",/* WindowHandler::GetInstance()->GetVSync() == VSync::On ? SDL_RENDERER_PRESENTVSYNC : */0);
        if(renderer == nullptr)
            BeeCoreError("Could not create renderer: {}", SDL_GetError());
        auto metalLayer = SDL_GetRenderMetalLayer(renderer);

        WGPUSurfaceDescriptorFromMetalLayer sfdMetal = {
                .chain =  {
                        .next =  nullptr,
                        .sType = WGPUSType::WGPUSType_SurfaceDescriptorFromMetalLayer
                },
                .layer =  metalLayer
        };
        WGPUSurfaceDescriptor sfd = {
                .nextInChain =  (WGPUChainedStruct*)&sfdMetal,
                .label =  nullptr
        };
        surface = wgpuInstanceCreateSurface(instance, &sfd);

        SDL_DestroyRenderer(renderer);
#endif
        return surface;
    }

    /**
    * Utility function to get a WebGPU device, so that
    *     WGPUAdapter device = requestDevice(adapter, options);
    * is roughly equivalent to
    *     const device = await adapter.requestDevice(descriptor);
    * It is very similar to requestAdapter
    */
    WGPUDevice WebGPUGraphicsDevice::RequestDevice(WGPUAdapter adapter, WGPUDeviceDescriptor const * descriptor)
    {
        struct UserData{
            WGPUDevice device = nullptr;
            bool requestEnded = false;
        };
        UserData userData;

        auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device, char const * message, void * pUserData) {
            UserData& userData = *reinterpret_cast<UserData*>(pUserData);
            if (status == WGPURequestDeviceStatus_Success) {
                userData.device = device;
            } else {
                BeeCoreError("Could not get WebGPU device: {}", message);
            }
            userData.requestEnded = true;
        };

        wgpuAdapterRequestDevice(
                adapter,
                descriptor,
                onDeviceRequestEnded,
                (void*)&userData
        );

        BeeEnsures(userData.requestEnded);

        return userData.device;
    }

    void WebGPUGraphicsDevice::SubmitCommandBuffers(CommandBuffer* commandBuffers, uint32_t numberOfBuffers)
    {
        std::vector<WGPUCommandBuffer> buffers;
        buffers.reserve(numberOfBuffers);
        WebGPUCommandBuffer* cmdBuffers = (WebGPUCommandBuffer*)commandBuffers;
        for(uint32_t i = 0; i < numberOfBuffers; i++)
        {
            WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
            cmdBufferDescriptor.nextInChain = nullptr;
            cmdBufferDescriptor.label = "Command buffer";
            WGPUCommandBuffer command = wgpuCommandEncoderFinish(cmdBuffers[i].GetHandle(), &cmdBufferDescriptor);
            buffers.push_back(command);
        }
        wgpuQueueSubmit(m_Queue, numberOfBuffers, buffers.data());
        for(uint32_t i = 0; i < numberOfBuffers; i++)
        {
            wgpuCommandEncoderRelease(cmdBuffers[i].GetHandle());
            wgpuCommandBufferRelease(buffers[i]);
        }
    }

    void WebGPUGraphicsDevice::WindowResized(uint32_t width, uint32_t height)
    {
        m_SwapChain = CreateScope<WebGPUSwapChain>(*this);
        m_SwapChainRequiresRebuild = false;
    }

    CommandBuffer WebGPUGraphicsDevice::CreateCommandBuffer()
    {
        WGPUCommandEncoderDescriptor encoderDesc = {};
        encoderDesc.nextInChain = nullptr;
        encoderDesc.label = "Command encoder";
        WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(m_Device, &encoderDesc);
        return {encoder};
    }

    void WebGPUGraphicsDevice::LogAdapterInfo() const noexcept
    {
        WGPUAdapterProperties properties {};
        wgpuAdapterGetProperties(m_Adapter, &properties);
        BeeCoreInfo("WebGPU adapter info:");
        BeeCoreInfo("  Name: {}", properties.name);
        BeeCoreInfo("  Vendor: {}", properties.vendorID);
        BeeCoreInfo("  DeviceID: {}", properties.deviceID);
        BeeCoreInfo("  BackendType: {}", ToString(properties.backendType));
        BeeCoreInfo("  AdapterType: {}", ToString(properties.adapterType));
        BeeCoreInfo("  DriverDescription: {}", properties.driverDescription);
    }

    WGPUBuffer WebGPUGraphicsDevice::CreateBuffer(WGPUBufferUsageFlags usage, uint32_t size)
    {
        WGPUBufferDescriptor bufferDesc = {};
        bufferDesc.nextInChain = nullptr;
        bufferDesc.label = "GPU-side data buffer";
        bufferDesc.usage = usage;
        bufferDesc.size = size;
        bufferDesc.mappedAtCreation = false;
        return wgpuDeviceCreateBuffer(m_Device, &bufferDesc);
    }

    void WebGPUGraphicsDevice::CopyDataToBuffer(gsl::span<byte> data, WGPUBuffer buffer)
    {
        wgpuQueueWriteBuffer(m_Queue, buffer, 0, data.data(), data.size());
    }

}
