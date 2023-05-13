//
// Created by alexl on 13.05.2023.
//

#include "Texture.h"
#include "Core/Logging/Log.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture2D.h"


namespace BeeEngine
{

    Ref<Texture2D> Texture2D::Create(const String &path)
    {
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                return CreateRef<OpenGLTexture2D>(path);
            default:
                BeeCoreError("Unknown RenderAPI");
                return nullptr;
        }
    }

    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
    {
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                return CreateRef<OpenGLTexture2D>(width, height);
            default:
                BeeCoreError("Unknown RenderAPI");
                return nullptr;
        }
    }
}
