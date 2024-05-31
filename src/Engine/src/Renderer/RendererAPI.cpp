//
// Created by Александр Лебедев on 07.05.2023.
//

#include "RendererAPI.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Platform/WebGPU/WebGPURendererAPI.h"
#include "Renderer.h"

namespace BeeEngine
{
    Scope<RendererAPI> RendererAPI::Create()
    {
        BEE_PROFILE_FUNCTION();
        switch (Renderer::GetAPI())
        {
#if defined(BEE_COMPILE_WEBGPU)
            case RenderAPI::WebGPU:
                return CreateRef<Internal::WebGPURendererAPI>();
#endif
#if defined(BEE_COMPILE_VULKAN)
            case RenderAPI::Vulkan:
                return CreateScope<Internal::VulkanRendererAPI>();
#endif
            default:
                BeeCoreFatalError("Renderer API not supported!");
                return nullptr;
        }
    }
} // namespace BeeEngine
