//
// Created by Александр Лебедев on 03.07.2023.
//

#include "WebGPUMesh.h"
#include "WebGPUGraphicsDevice.h"

namespace BeeEngine::Internal
{

    WebGPUMesh::WebGPUMesh(in<std::vector<Vertex>> vertices, BufferLayout& layout)
    : m_Layout(layout), m_GraphicsDevice(WebGPUGraphicsDevice::GetInstance())
    {
        InitBufferLayout();
        CreateVertexBuffers(vertices);
    }

    WebGPUMesh::~WebGPUMesh()
    {
        wgpuBufferDestroy(m_Buffer);
        wgpuBufferRelease(m_Buffer);
    }

    void WebGPUMesh::InitBufferLayout()
    {

    }

    void WebGPUMesh::CreateVertexBuffers(in<std::vector<Vertex>> data)
    {
        auto size = data.size() * sizeof(Vertex);
        m_Buffer = m_GraphicsDevice.CreateBuffer(WGPUBufferUsage_Vertex, size);
        m_GraphicsDevice.CopyDataToBuffer({(byte*)data.data(), size}, m_Buffer);
    }
}