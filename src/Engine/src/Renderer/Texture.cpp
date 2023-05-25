//
// Created by alexl on 13.05.2023.
//

#include "Texture.h"
#include "Core/Logging/Log.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture2D.h"


namespace BeeEngine
{

    Ref<Texture2D> Texture2D::Create(std::string_view path)
    {
        BEE_PROFILE_FUNCTION();
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                return CreateRef<Internal::OpenGLTexture2D>(path);
            default:
                BeeCoreError("Unknown RenderAPI");
                throw std::exception();
        }
    }

    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
    {
        BEE_PROFILE_FUNCTION();
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                return CreateRef<Internal::OpenGLTexture2D>(width, height);
            default:
                BeeCoreError("Unknown RenderAPI");
                throw std::exception();
        }
    }
}
