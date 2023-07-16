//
// Created by alexl on 13.05.2023.
//

#include "Texture.h"
#include "Core/Logging/Log.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture2D.h"
#include "Platform/WebGPU/WebGPUTexture2D.h"


namespace BeeEngine
{

    Ref<Texture2D> Texture2D::Create(std::string_view path)
    {
        BEE_PROFILE_FUNCTION();
        switch (Renderer::GetAPI())
        {
            case RenderAPI::WebGPU:
                return CreateRef<Internal::WebGPUTexture2D>(path);
            case RenderAPI::OpenGL:
                //return CreateRef<Internal::OpenGLTexture2D>(path);
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
            case RenderAPI::WebGPU:
                return CreateRef<Internal::WebGPUTexture2D>(width, height);
            case RenderAPI::OpenGL:
                //return CreateRef<Internal::OpenGLTexture2D>(width, height);
            default:
                BeeCoreError("Unknown RenderAPI");
                throw std::exception();
        }
    }

    Ref<Texture2D> Texture2D::CreateFromMemory(gsl::span<std::byte> data)
    {
        BEE_PROFILE_FUNCTION();
        switch (Renderer::GetAPI())
        {
            case RenderAPI::WebGPU:
                return CreateRef<Internal::WebGPUTexture2D>(data);
            case RenderAPI::OpenGL:
                //return CreateRef<Internal::OpenGLTexture2D>(data);
            default:
                BeeCoreError("Unknown RenderAPI");
                throw std::exception();
        }
    }
}
