//
// Created by Александр Лебедев on 03.07.2023.
//

#pragma once
#include "Renderer/Mesh.h"
#include <webgpu/webgpu.h>

namespace BeeEngine::Internal
{
    class WebGPUMesh: public Mesh
    {
    public:
        WebGPUMesh(in<std::vector<Vertex>> vertices);
        WebGPUMesh(in<std::vector<Vertex>> vertices, in<std::vector<uint32_t>> indices);

        WebGPUMesh(void* data, size_t size, size_t vertexCount, const std::vector<uint32_t>& indices);
        ~WebGPUMesh() override;
        WebGPUMesh(const WebGPUMesh& other) = delete;
        WebGPUMesh& operator=(const WebGPUMesh& other ) = delete;
        WebGPUMesh(WebGPUMesh&& other) noexcept = default;
        WebGPUMesh& operator=(WebGPUMesh&& other) noexcept = default;
        [[nodiscard]] bool IsIndexed() const override
        {
            return m_IndexBuffer != nullptr;
        }
        [[nodiscard]] uint32_t GetVertexCount() const override
        {
            return m_VertexCount;
        }
        [[nodiscard]] uint32_t GetIndexCount() const override
        {
            return m_IndexCount;
        }
        void Bind(void* commandBuffer) override;
    private:
        void CreateVertexBuffers(void* data);
        void CreateIndexBuffers(in<std::vector<uint32_t>> data);

        WGPUBuffer m_VertexBuffer;
        WGPUBuffer m_IndexBuffer = nullptr;
        uint32_t m_VertexCount;
        uint32_t m_IndexCount = 0;
        size_t m_Size;
        size_t m_IndexSize;

        BufferLayout m_Layout;
        class WebGPUGraphicsDevice& m_GraphicsDevice;
    };
}
