//
// Created by alexl on 13.05.2023.
//

#include "VertexArray.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"


namespace BeeEngine
{

    Ref<VertexArray> VertexArray::Create(Ref<GraphicsBuffer> vertexBuffer, Ref<GraphicsBuffer> indexBuffer)
    {
        BEE_PROFILE_FUNCTION();
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                return CreateRef<Internal::OpenGLVertexArray>(vertexBuffer, indexBuffer);
            default:
                BeeCoreError("Unknown RenderAPI");
                return nullptr;
        }
    }
}
