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
        WebGPUMesh(in<std::vector<Vertex>> vertices, BufferLayout& layout);
        ~WebGPUMesh() override;
        WebGPUMesh(const WebGPUMesh& other) = delete;
        WebGPUMesh& operator=(const WebGPUMesh& other ) = delete;
    private:
        void InitBufferLayout();
        void CreateVertexBuffers(in<std::vector<Vertex>> data);

        WGPUBuffer m_Buffer;

        BufferLayout m_Layout;
        class WebGPUGraphicsDevice& m_GraphicsDevice;
    };
}
