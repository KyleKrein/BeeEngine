//
// Created by alexl on 13.05.2023.
//

#include "GraphicsBuffer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
//#include "Platform/Vulkan/VulkanVertexBuffer.h"


namespace BeeEngine
{

    Ref<GraphicsBuffer> GraphicsBuffer::CreateIndexBuffer(gsl::span<uint32_t> data)
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(!data.empty());
        gsl::span<std::byte> byteData = {reinterpret_cast<std::byte*>(data.data()), data.size_bytes()};
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                return CreateRef<Internal::OpenGLIndexBuffer>(byteData);
            default:
                BeeCoreFatalError("Unknown RenderAPI!");
        }
    }

    Ref<GraphicsBuffer> GraphicsBuffer::CreateVertexBuffer(uint32_t size)
    {
        BEE_PROFILE_FUNCTION();
        Ref<GraphicsBuffer> result;
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                //result = CreateRef<Internal::OpenGLVertexBuffer>(size);
                return result;
            default:
                BeeCoreFatalError("Unknown RenderAPI!");
        }
    }
    Ref<GraphicsBuffer> GraphicsBuffer::CreateVertexBuffer(uint32_t numberOfVertices, in<BufferLayout> layout)
    {
        BEE_PROFILE_FUNCTION();
        Ref<GraphicsBuffer> result;
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                result = CreateRef<Internal::OpenGLVertexBuffer>(numberOfVertices);
                result->SetLayout(layout);
                return result;
            case RenderAPI::Vulkan:
                //result = CreateRef<Internal::VulkanVertexBuffer>(numberOfVertices, layout);
                return result;
            default:
            BeeCoreFatalError("Unknown RenderAPI!");
        }
    }

    GraphicsBuffer::GraphicsBuffer()
    = default;
}
