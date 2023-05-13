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

    Ref<GraphicsBuffer> GraphicsBuffer::CreateIndexBuffer(uint32_t size)
    {
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                return CreateRef<OpenGLIndexBuffer>(size);
            default:
                return nullptr;
        }
    }

    Ref<GraphicsBuffer> GraphicsBuffer::CreateVertexBuffer(uint32_t size)
    {
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                return CreateRef<OpenGLVertexBuffer>(size);
            default:
                return nullptr;
        }
    }
}
