//
// Created by Александр Лебедев on 27.06.2023.
//
#include "Mesh.h"

#include "Renderer.h"
#include "Platform/Vulkan/VulkanMesh.h"
#include "Platform/WebGPU/WebGPUMesh.h"

namespace BeeEngine
{
    Ref<Mesh> Mesh::Create(in<std::vector<Vertex>> vertices)
    {
        switch (Renderer::GetAPI())
        {
            case WebGPU:
                return CreateRef<Internal::WebGPUMesh>(vertices);
#if defined(BEE_COMPILE_VULKAN)
            case Vulkan:
                return CreateRef<Internal::VulkanMesh>(vertices);
#endif
            default:
                BeeCoreError("Unknown API!");
            return nullptr;
        }
    }
    Ref<Mesh> Mesh::Create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    {
        switch (Renderer::GetAPI())
        {
            case WebGPU:
                return CreateRef<Internal::WebGPUMesh>(vertices, indices);
#if defined(BEE_COMPILE_VULKAN)
            case Vulkan:
                return CreateRef<Internal::VulkanMesh>(vertices, indices);
#endif
            default:
                BeeCoreError("Unknown API!");
                return nullptr;
        }
    }

    Ref<Mesh> Mesh::Create(void *verticesData, size_t size, size_t vertexCount, const std::vector<uint32_t> &indices)
    {
        switch (Renderer::GetAPI())
        {
            case WebGPU:
                return CreateRef<Internal::WebGPUMesh>(verticesData, size, vertexCount, indices);
#if defined(BEE_COMPILE_VULKAN)
            case Vulkan:
                return CreateRef<Internal::VulkanMesh>(verticesData, size, vertexCount, indices);
#endif
            default:
                BeeCoreError("Unknown API!");
            return nullptr;
        }
    }
}