//
// Created by alexl on 17.07.2023.
//

#include "WebGPUGraphicsDevice.h"
#include "Core/DeletionQueue.h"
#include "WebGPUBufferPool.h"

namespace BeeEngine::Internal
{

    const WebGPUBuffer &WebGPUBufferPool::RequestBuffer(uint32_t size, WebGPUBufferUsage usage)
    {
        //TODO implement actual pool
        m_Buffers.push_back({CreateBuffer(size, usage), size, 0, usage});
        return m_Buffers.back();
    }

    void WebGPUBufferPool::ReleaseBuffer(const WebGPUBuffer &buffer)
    {
        DestroyBuffer(buffer);
        std::remove(m_Buffers.begin(), m_Buffers.end(), buffer);
    }

    WGPUBuffer WebGPUBufferPool::CreateBuffer(uint32_t size, WebGPUBufferUsage usage)
    {
        WGPUBufferUsageFlags flags = 0;
        switch (usage)
        {
            case WebGPUBufferUsage::Vertex:
                flags = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst;
                BeeCoreTrace("Allocating vertex buffer of size {}", size);
                break;
        }
        return WebGPUGraphicsDevice::GetInstance().CreateBuffer(flags, size);
    }

    void WebGPUBufferPool::DestroyBuffer(const WebGPUBuffer &buffer)
    {
        wgpuBufferDestroy(buffer.Buffer);
        wgpuBufferRelease(buffer.Buffer);
    }

    WebGPUBufferPool::WebGPUBufferPool()
    {
        DeletionQueue::Main().PushFunction([this]()
        {
            for (auto& buffer : m_Buffers)
            {
                DestroyBuffer(buffer);
            }
            m_Buffers.clear();
        });
    }

    WebGPUBufferPool::~WebGPUBufferPool()
    {

    }
}
