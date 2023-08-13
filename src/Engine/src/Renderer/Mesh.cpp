//
// Created by Александр Лебедев on 27.06.2023.
//
#include "Mesh.h"
#include "Platform/Vulkan/VulkanModel.h"
#include "Platform/WebGPU/WebGPUMesh.h"

namespace BeeEngine
{
    Ref<Mesh> Mesh::Create(in<std::vector<Vertex>> vertices)
    {
        return CreateRef<Internal::WebGPUMesh>(vertices/*gsl::span<byte>{(byte*)vertices.data(), vertices.size() * sizeof(Vertex)}*/);
    }
    Ref<Mesh> Mesh::Create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    {
        return CreateRef<Internal::WebGPUMesh>(vertices, indices);
    }

    Ref<Mesh> Mesh::Create(void *verticesData, size_t size, size_t vertexCount, const std::vector<uint32_t> &indices)
    {
        return CreateRef<Internal::WebGPUMesh>(verticesData, size, vertexCount, indices);
    }
}