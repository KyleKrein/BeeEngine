//
// Created by alexl on 12.07.2023.
//
#if defined(BEE_COMPILE_WEBGPU)
#include "WebGPUInstancedBuffer.h"
#include "WebGPUGraphicsDevice.h"
#include "Renderer/RenderPass.h"

namespace BeeEngine::Internal
{

    WebGPUInstancedBuffer::WebGPUInstancedBuffer(in<WGPUVertexBufferLayout> shaderModule, size_t maxInstances)
    : m_GraphicsDevice(WebGPUGraphicsDevice::GetInstance()), m_Size(maxInstances * shaderModule.arrayStride), m_MaxInstances(maxInstances), m_OneInstanceSize(shaderModule.arrayStride),
      m_Buffer(m_GraphicsDevice.RequestBuffer(m_Size, WebGPUBufferUsage::Vertex))
    {}

    WebGPUInstancedBuffer::~WebGPUInstancedBuffer()
    {
        m_GraphicsDevice.ReleaseBuffer(m_Buffer);
    }

    void WebGPUInstancedBuffer::SetData(void *data, size_t size)
    {
        BeeExpects(size <= m_Size);
        m_GraphicsDevice.CopyDataToBuffer({(byte*)data, size}, m_Buffer.Buffer);
    }

    void WebGPUInstancedBuffer::Bind(void* cmd)
    {
        wgpuRenderPassEncoderSetVertexBuffer((WGPURenderPassEncoder)(((RenderPass*)cmd)->GetHandle()), 1, m_Buffer.Buffer, 0, m_Size);
    }

    WebGPUInstancedBuffer::WebGPUInstancedBuffer(size_t size)
    : m_GraphicsDevice(WebGPUGraphicsDevice::GetInstance()), m_Size(size),
      m_Buffer(m_GraphicsDevice.RequestBuffer(m_Size, WebGPUBufferUsage::Vertex))
    {

    }
}
#endif