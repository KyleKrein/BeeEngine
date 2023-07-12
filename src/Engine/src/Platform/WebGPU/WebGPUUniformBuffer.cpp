//
// Created by alexl on 11.07.2023.
//

#include "WebGPUUniformBuffer.h"
#include "WebGPUGraphicsDevice.h"
#include "Core/DeletionQueue.h"

namespace BeeEngine::Internal
{

    WebGPUUniformBuffer::WebGPUUniformBuffer(size_t size)
    : m_Size(size), m_GraphicsDevice(WebGPUGraphicsDevice::GetInstance())
    {
        m_Buffer = m_GraphicsDevice.CreateBuffer(WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform, m_Size);
    }

    WebGPUUniformBuffer::~WebGPUUniformBuffer()
    {
        auto buffer = m_Buffer;
        DeletionQueue::Frame().PushFunction([buffer](){
            wgpuBufferDestroy(buffer);
            wgpuBufferRelease(buffer);
        });
    }

    void WebGPUUniformBuffer::SetData(void *data, size_t size)
    {
        m_GraphicsDevice.CopyDataToBuffer({(byte*)data, size}, m_Buffer);
    }

    void WebGPUUniformBuffer::Bind(uint32_t binding)
    {

    }
}