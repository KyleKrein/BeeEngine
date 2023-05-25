//
// Created by Александр Лебедев on 07.05.2023.
//

#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include "Renderer.h"

namespace BeeEngine
{
    Ref<RendererAPI> RendererAPI::Create()
    {
        BEE_PROFILE_FUNCTION();
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                return CreateRef<Internal::OpenGLRendererAPI>();
            default:
                BeeCoreFatalError("Renderer API not supported!");
                return nullptr;
        }
    }
}
