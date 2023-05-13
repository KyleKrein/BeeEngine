//
// Created by alexl on 13.05.2023.
//

#include "Shader.h"
#include "Core/Logging/Log.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"


namespace BeeEngine
{

    Ref<Shader> Shader::Create(const String &name, const String &vertexSrc, const String &fragmentSrc)
    {
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
            default:
                BeeCoreError("Unknown RenderAPI");
                return nullptr;
        }
    }
}
