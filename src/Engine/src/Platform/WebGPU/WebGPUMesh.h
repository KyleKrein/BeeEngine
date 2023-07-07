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
        ~WebGPUMesh() override;
        WebGPUMesh(const WebGPUMesh& other) = delete;
        WebGPUMesh& operator=(const WebGPUMesh& other ) = delete;
        [[nodiscard]] uint32_t GetVertexCount() const override
        {
            return m_VertexCount;
        }
        void Bind(void* commandBuffer) override;
    private:
        void CreateVertexBuffers(in<std::vector<Vertex>> data);

        WGPUBuffer m_Buffer;
        uint32_t m_VertexCount;
        size_t m_Size;

        BufferLayout m_Layout;
        class WebGPUGraphicsDevice& m_GraphicsDevice;
    };
}
