//
// Created by alexl on 11.07.2023.
//

#include "UniformBuffer.h"

#include "Platform/Vulkan/VulkanUniformBuffer.h"
#include "Platform/WebGPU/WebGPUUniformBuffer.h"
#include "Renderer.h"

namespace BeeEngine
{
    Scope<UniformBuffer> UniformBuffer::Create(size_t size)
    {
        switch (Renderer::GetAPI())
        {
#if defined(BEE_COMPILE_WEBGPU)
            case WebGPU:
                return CreateRef<Internal::WebGPUUniformBuffer>(size);
#endif

#if defined(BEE_COMPILE_VULKAN)
            case Vulkan:
                return CreateScope<Internal::VulkanUniformBuffer>(size);
#endif
        }
        BeeCoreError("Unknown RendererAPI!");
        return nullptr;
    }
} // namespace BeeEngine
