//
// Created by Александр Лебедев on 30.06.2023.
//

#include "WebGPUInstance.h"
#include "Core/CodeSafety/Expects.h"
#if defined(BEE_COMPILE_WEBGPU)
namespace BeeEngine::Internal
{

    WebGPUInstance::WebGPUInstance()
    {
        WGPUInstanceDescriptor descriptor = {};
        descriptor.nextInChain = nullptr;
        m_Instance = wgpuCreateInstance(&descriptor);

        BeeCoreAssert(m_Instance != nullptr, "Failed to create WebGPU instance");
    }

    WebGPUInstance::~WebGPUInstance()
    {
        wgpuInstanceRelease(m_Instance);
    }
} // namespace BeeEngine::Internal
#endif