//
// Created by alexl on 17.07.2023.
//
#include "InstancedBuffer.h"

#include "Platform/Vulkan/VulkanInstancedBuffer.h"
#include "Platform/WebGPU/WebGPUInstancedBuffer.h"
#include "Renderer.h"

BeeEngine::Scope<BeeEngine::InstancedBuffer> BeeEngine::InstancedBuffer::Create(size_t size)
{
    switch (Renderer::GetAPI())
    {
#if defined(BEE_COMPILE_WEBGPU)
        case WebGPU:
            return BeeEngine::CreateScope<BeeEngine::Internal::WebGPUInstancedBuffer>(size);
#endif
#if defined(BEE_COMPILE_VULKAN)
        case Vulkan:
            return BeeEngine::CreateScope<BeeEngine::Internal::VulkanInstancedBuffer>(size);
#endif
    }
    BeeCoreError("Unknown RendererAPI!");
    return nullptr;
}
