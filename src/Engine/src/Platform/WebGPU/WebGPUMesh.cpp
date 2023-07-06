//
// Created by Александр Лебедев on 03.07.2023.
//

#include "WebGPUMesh.h"
#include "WebGPUGraphicsDevice.h"
#include "Renderer/RenderPass.h"

namespace BeeEngine::Internal
{

    WebGPUMesh::WebGPUMesh(in<std::vector<Vertex>> vertices)
    : m_GraphicsDevice(WebGPUGraphicsDevice::GetInstance()), m_VertexCount(vertices.size())
    {
        CreateVertexBuffers(vertices);
    }

    WebGPUMesh::~WebGPUMesh()
    {
        wgpuBufferDestroy(m_Buffer);
        wgpuBufferRelease(m_Buffer);
    }

    void WebGPUMesh::CreateVertexBuffers(in<std::vector<Vertex>> data)
    {
        m_Size = data.size() * sizeof(Vertex);
        m_Buffer = m_GraphicsDevice.CreateBuffer(WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex, m_Size);
        m_GraphicsDevice.CopyDataToBuffer({(byte*)data.data(), m_Size}, m_Buffer);
    }

    void WebGPUMesh::Bind(void *commandBuffer)
    {
        wgpuRenderPassEncoderSetVertexBuffer((WGPURenderPassEncoder)(((RenderPass*)commandBuffer)->GetHandle()), 0, m_Buffer, 0, m_Size);
    }
}