//
// Created by alexl on 13.05.2023.
//

#include "GraphicsBuffer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"


namespace BeeEngine
{

    Ref<GraphicsBuffer> GraphicsBuffer::CreateIndexBuffer(uint32_t data[], uint32_t size)
    {
        BEE_PROFILE_FUNCTION();
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                return CreateRef<Internal::OpenGLIndexBuffer>(data, size);
            default:
                return nullptr;
        }
    }

    Ref<GraphicsBuffer> GraphicsBuffer::CreateVertexBuffer(uint32_t size)
    {
        BEE_PROFILE_FUNCTION();
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                return CreateRef<Internal::OpenGLVertexBuffer>(size);
            default:
                return nullptr;
        }
    }

    GraphicsBuffer::GraphicsBuffer()
    {

    }
}
