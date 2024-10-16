//
// Created by Александр Лебедев on 27.06.2023.
//
#include "Mesh.h"

#include "Platform/Vulkan/VulkanMesh.h"
#include "Platform/WebGPU/WebGPUMesh.h"
#include "Renderer.h"

namespace BeeEngine
{
    Ref<Mesh> Mesh::Create(in<std::vector<Vertex>> vertices)
    {
        switch (Renderer::GetAPI())
        {
#if defined(BEE_COMPILE_WEBGPU)
            case WebGPU:
                return CreateRef<Internal::WebGPUMesh>(vertices);
#endif
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
#if defined(BEE_COMPILE_WEBGPU)
            case WebGPU:
                return CreateRef<Internal::WebGPUMesh>(vertices, indices);
#endif
#if defined(BEE_COMPILE_VULKAN)
            case Vulkan:
                return CreateRef<Internal::VulkanMesh>(vertices, indices);
#endif
            default:
                BeeCoreError("Unknown API!");
                return nullptr;
        }
    }

    Ref<Mesh> Mesh::Create(void* verticesData, size_t size, size_t vertexCount, const std::vector<uint32_t>& indices)
    {
        switch (Renderer::GetAPI())
        {
#if defined(BEE_COMPILE_WEBGPU)
            case WebGPU:
                return CreateRef<Internal::WebGPUMesh>(verticesData, size, vertexCount, indices);
#endif
#if defined(BEE_COMPILE_VULKAN)
            case Vulkan:
                return CreateRef<Internal::VulkanMesh>(verticesData, size, vertexCount, indices);
#endif
            default:
                BeeCoreError("Unknown API!");
                return nullptr;
        }
    }
} // namespace BeeEngine