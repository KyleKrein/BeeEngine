//
// Created by Александр Лебедев on 03.07.2023.
//

#include "WebGPUMesh.h"
#include "WebGPUGraphicsDevice.h"
#include "Renderer/RenderPass.h"

namespace BeeEngine::Internal
{

    WebGPUMesh::WebGPUMesh(in<std::vector<Vertex>> vertices)
    : m_GraphicsDevice(WebGPUGraphicsDevice::GetInstance()), m_VertexCount(vertices.size()), m_Size(vertices.size() * sizeof(Vertex))
    {
        CreateVertexBuffers((void*)vertices.data());
    }
    WebGPUMesh::WebGPUMesh(in<std::vector<Vertex>> vertices, in<std::vector<uint32_t>> indices)
    : m_GraphicsDevice(WebGPUGraphicsDevice::GetInstance()), m_VertexCount(vertices.size()), m_IndexCount(indices.size()), m_IndexSize(indices.size() * sizeof(uint32_t)), m_Size(vertices.size() * sizeof(Vertex))
    {
        CreateVertexBuffers((void*)vertices.data());
        CreateIndexBuffers(indices);
    }

    WebGPUMesh::~WebGPUMesh()
    {
        wgpuBufferDestroy(m_VertexBuffer);
        wgpuBufferRelease(m_VertexBuffer);
        if(WebGPUMesh::IsIndexed())
        {
            wgpuBufferDestroy(m_IndexBuffer);
            wgpuBufferRelease(m_IndexBuffer);
        }
    }
    void WebGPUMesh::CreateVertexBuffers(void* data)
    {
        m_VertexBuffer = m_GraphicsDevice.CreateBuffer(WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex, m_Size);
        m_GraphicsDevice.CopyDataToBuffer({(byte*)data, m_Size}, m_VertexBuffer);
    }
    void WebGPUMesh::CreateIndexBuffers(in<std::vector<uint32_t>> data)
    {
        m_IndexBuffer = m_GraphicsDevice.CreateBuffer(WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index, m_IndexSize);
        m_GraphicsDevice.CopyDataToBuffer({(byte*)data.data(), m_IndexSize}, m_IndexBuffer);
    }

    void WebGPUMesh::Bind(void *commandBuffer)
    {
        wgpuRenderPassEncoderSetVertexBuffer((WGPURenderPassEncoder)(((RenderPass*)commandBuffer)->GetHandle()), 0, m_VertexBuffer, 0, m_Size);
        if(IsIndexed())
        {
            wgpuRenderPassEncoderSetIndexBuffer((WGPURenderPassEncoder)(((RenderPass*)commandBuffer)->GetHandle()), m_IndexBuffer, WGPUIndexFormat_Uint32, 0, m_IndexSize);
        }
    }

    WebGPUMesh::WebGPUMesh(void *data, size_t size, size_t vertexCount, const std::vector<uint32_t> &indices)
            : m_GraphicsDevice(WebGPUGraphicsDevice::GetInstance()), m_VertexCount(vertexCount), m_IndexCount(indices.size()), m_IndexSize(indices.size() * sizeof(uint32_t)), m_Size(size)
    {
        CreateVertexBuffers(data);
        CreateIndexBuffers(indices);
    }
}