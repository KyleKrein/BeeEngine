//
// Created by alexl on 13.05.2023.
//
#include "Shader.h"
#include "Core/Logging/Log.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"


namespace BeeEngine
{

    Ref<Shader> Shader::Create(std::string_view name, std::string_view vertexSrc, std::string_view fragmentSrc)
    {
        BEE_PROFILE_FUNCTION();
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                return CreateRef<Internal::OpenGLShader>(name, vertexSrc, fragmentSrc);
            default:
                BeeCoreFatalError("Unknown RenderAPI");
                return nullptr;
        }
    }

    Ref<Shader> Shader::Create(std::string_view name, std::string_view filepath)
    {
        BEE_PROFILE_FUNCTION();
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                return CreateRef<Internal::OpenGLShader>(name, filepath);
            default:
                BeeCoreFatalError("Unknown RenderAPI");
                return nullptr;
        }
    }


}
