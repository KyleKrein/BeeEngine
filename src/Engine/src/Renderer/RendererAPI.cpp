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
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                return CreateRef<OpenGLRendererAPI>();
            default:
                BeeCoreAssert(false, "Renderer API not supported!");
                return nullptr;
        }
    }
}
