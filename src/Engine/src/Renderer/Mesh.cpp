//
// Created by Александр Лебедев on 27.06.2023.
//
#include "Mesh.h"
#include "Platform/Vulkan/VulkanModel.h"
namespace BeeEngine
{
    Ref<Mesh> Mesh::Create(in<std::vector<Vertex>> vertices, BufferLayout layout)
    {
        return CreateRef<Internal::VulkanModel>(Internal::VulkanGraphicsDevice::GetInstance(), gsl::span<byte>{(byte*)vertices.data(), vertices.size() * sizeof(Vertex)}, vertices.size(), std::move(layout));
    }
}