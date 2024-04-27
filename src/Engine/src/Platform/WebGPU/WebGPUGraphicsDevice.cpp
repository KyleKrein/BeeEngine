//
// Created by Александр Лебедев on 30.06.2023.
//
#if defined(BEE_COMPILE_WEBGPU)
#include "WebGPUGraphicsDevice.h"
#if defined(BEE_COMPILE_SDL)
#include "SDL3/SDL.h"
#endif
#include "Windowing/WindowHandler/WindowHandler.h"
#include "Core/TypeDefines.h"
#include "Renderer/Vertex.h"
#include "Core/DeletionQueue.h"
#include <Core/Coroutines/Co_Promise.h>
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
        adapterOpts.compatibilityMode = false; //don't need to be compatible with WebGL
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
        WGPUSupportedLimits supportedLimits;

        wgpuAdapterGetLimits(m_Adapter, &supportedLimits);
        m_SupportedLimits = supportedLimits;
        WGPUChainedStruct dawnTogglesChained = {};
        dawnTogglesChained.sType = WGPUSType_DawnTogglesDescriptor;
        dawnTogglesChained.next = nullptr;

        WGPUDawnTogglesDescriptor dawnToggles = {};
        std::vector<const char*> enabledToggles;

#if defined(BEE_ENABLE_GRAPHICS_DEBUGGING)
        enabledToggles.push_back("disable_symbol_renaming");
        enabledToggles.push_back("dump_shaders");
#endif
        dawnToggles.enabledTogglesCount = enabledToggles.size();
        dawnToggles.enabledToggles = enabledToggles.data();
        dawnToggles.disabledToggles = nullptr;
        dawnToggles.disabledTogglesCount = 0;
        dawnToggles.chain = dawnTogglesChained;

        WGPUDeviceDescriptor deviceDescriptor = {};
        deviceDescriptor.nextInChain = &dawnToggles.chain;
        deviceDescriptor.label = "BeeEngine Device"; // anything works here, that's your call
        deviceDescriptor.requiredFeaturesCount = 0; // we do not require any specific feature
        deviceDescriptor.requiredFeatures = nullptr;

#if 0
        WGPURequiredLimits requiredLimits{};
        SetDefault(requiredLimits.limits);
        requiredLimits.limits.maxVertexAttributes = 1;
        requiredLimits.limits.maxVertexBuffers = 1;
        requiredLimits.limits.maxBufferSize = sizeof(Vertex) * 1024;
        requiredLimits.limits.maxVertexBufferArrayStride = supportedLimits.limits.maxVertexBufferArrayStride;
        requiredLimits.limits.minStorageBufferOffsetAlignment = supportedLimits.limits.minStorageBufferOffsetAlignment;
        deviceDescriptor.requiredLimits = &requiredLimits;
#else
        deviceDescriptor.requiredLimits = nullptr; // we do not require any specific limit
#endif
        deviceDescriptor.defaultQueue.nextInChain = nullptr;
        deviceDescriptor.defaultQueue.label = "The default queue";

        m_Device = RequestDevice(m_Adapter, &deviceDescriptor);

        wgpuDeviceGetLimits(m_Device, &supportedLimits);
        BeeCoreTrace("device.maxVertexAttributes: {}", supportedLimits.limits.maxVertexAttributes);

        auto onDeviceError = [](WGPUErrorType type, char const* message, void* pUserData)
        {
            BeeCoreError("Uncaptured device error: type {0}, message: {1}", ToString(type), message? message : "null");
#if defined(BEE_ENABLE_ASSERTS)
            debug_break();
#endif
        };
        auto onDeviceLost = [](WGPUDeviceLostReason reason, char const* message, void* pUserData)
        {
            BeeCoreError("Device lost: reason {0}, message: {1}", ToString(reason), message? message : "null");
#if defined(BEE_ENABLE_ASSERTS)
            if(reason != WGPUDeviceLostReason::WGPUDeviceLostReason_Destroyed)
                debug_break();
#endif
        };
        auto onDeviceLogging = [](WGPULoggingType type, char const* message, void* pUserData)
        {
            BeeCoreInfo("Device logging: type {0}, message: {1}", ToString(type), message? message : "null");
        };
        wgpuDeviceSetUncapturedErrorCallback(m_Device, onDeviceError, nullptr /* pUserData */);
        wgpuDeviceSetLoggingCallback(m_Device, onDeviceLogging, nullptr /* pUserData */);
        wgpuDeviceSetDeviceLostCallback(m_Device, onDeviceLost, nullptr /* pUserData */);

        auto device = m_Device;
        DeletionQueue::Main().PushFunction([device]() {
            wgpuDeviceTick(device);
        });

        m_Queue = wgpuDeviceGetQueue(m_Device);
        m_BufferPool = CreateScope<WebGPUBufferPool>();

        m_SwapChain = CreateScope<WebGPUSwapChain>(*this);
    }

    WebGPUGraphicsDevice::~WebGPUGraphicsDevice()
    {
        m_BufferPool.reset();
        m_SwapChain.reset();
        wgpuQueueRelease(m_Queue);
        wgpuDeviceDestroy(m_Device);
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
        WGPUSurface surface;
#if defined(BEE_COMPILE_SDL)
        SDL_Window* window = (SDL_Window*)WindowHandler::GetInstance()->GetWindow();
#endif
#if defined(WINDOWS)
        auto nativeInfo = WindowHandler::GetInstance()->GetNativeInfo();
        WGPUSurfaceDescriptorFromWindowsHWND sfdHwnd = {
                .chain =  {
                        .next =  nullptr,
                        .sType =  WGPUSType_SurfaceDescriptorFromWindowsHWND
                },
                .hinstance =  nativeInfo.instance,
                .hwnd =  nativeInfo.window
        };
        WGPUSurfaceDescriptor sfd = {
                .nextInChain =  (WGPUChainedStruct*)&sfdHwnd,
                .label =  "SDL Window"
        };
        surface = wgpuInstanceCreateSurface(instance, &sfd);
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
        SDL_Renderer* renderer = SDL_CreateRenderer(window, "metal",/* WindowHandler::GetInstance()->GetVSync() == VSync::On ? SDL_RENDERER_PRESENTVSYNC : */0);
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

    Task<> WebGPUGraphicsDevice::WaitForQueueIdle()
    {
        Co_Promise<void> promise;
        auto future = promise.get_future();
        wgpuQueueOnSubmittedWorkDone(m_Queue, 0, [](WGPUQueueWorkDoneStatus status, void* userdata)
        {
            auto* p = (Co_Promise<void>*)userdata;
            p->set_value();
        }, &promise);
        while (!future.await_ready())
        {
            wgpuDeviceTick(m_Device);
        }
        co_return;
    }
}
#endif