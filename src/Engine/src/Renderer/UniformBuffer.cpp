//
// Created by alexl on 11.07.2023.
//

#include "UniformBuffer.h"

#include "Renderer.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"
#include "Platform/WebGPU/WebGPUUniformBuffer.h"


namespace BeeEngine
{
    Ref<UniformBuffer> UniformBuffer::Create(size_t size)
    {
        switch (Renderer::GetAPI())
        {
            case WebGPU:
                return CreateRef<Internal::WebGPUUniformBuffer>(size);
#if defined(BEE_COMPILE_VULKAN)
            case Vulkan:
                return CreateRef<Internal::VulkanUniformBuffer>(size);
#endif
        }
        BeeCoreError("Unknown RendererAPI!");
        return nullptr;
    }
}
