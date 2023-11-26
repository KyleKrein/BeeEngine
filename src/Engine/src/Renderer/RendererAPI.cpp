//
// Created by Александр Лебедев on 07.05.2023.
//

#include "RendererAPI.h"
#include "Renderer.h"
#include "Platform/WebGPU/WebGPURendererAPI.h"

namespace BeeEngine
{
    Ref<RendererAPI> RendererAPI::Create()
    {
        BEE_PROFILE_FUNCTION();
        switch (Renderer::GetAPI())
        {
            case RenderAPI::WebGPU:
                return CreateRef<Internal::WebGPURendererAPI>();
            default:
                BeeCoreFatalError("Renderer API not supported!");
                return nullptr;
        }
    }
}
