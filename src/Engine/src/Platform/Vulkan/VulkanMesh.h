//
// Created by Aleksandr on 24.02.2024.
//

#pragma once
#include "Renderer/Mesh.h"
#include "VulkanBuffer.h"
#include "VulkanGraphicsDevice.h"

namespace BeeEngine::Internal
{
    struct VulkanAccelerationStructure
    {
        vk::AccelerationStructureKHR AccelerationStructure;
        VulkanBuffer Buffer;
    };
    class VulkanMesh : public Mesh
    {
    public:
        ~VulkanMesh() override;

        [[nodiscard]] uint32_t GetVertexCount() const override;

        [[nodiscard]] uint32_t GetIndexCount() const override;

        void Bind(CommandBuffer& commandBuffer) override;

        [[nodiscard]] bool IsIndexed() const override;

        VulkanMesh(const std::vector<Vertex>& vertices);
        VulkanMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
        VulkanMesh(void* verticesData, size_t size, size_t vertexCount, const std::vector<uint32_t>& indices);

    private:
        void CreateVertexBuffer(const std::vector<Vertex>& vertices);
        void CreateVertexBuffer(const void* verticesData, size_t size, size_t vertexCount);
        void CreateIndexBuffer(const std::vector<uint32_t>& indices);
        void CreateAccelerationStructure(size_t vertexStride);

    private:
        VulkanGraphicsDevice& m_Device;
        VulkanBuffer m_VertexBuffer;
        VulkanBuffer m_IndexBuffer;
        size_t m_VertexCount;
        size_t m_IndexCount;
        VulkanAccelerationStructure m_AccelerationStructure;
    };
} // namespace BeeEngine::Internal
