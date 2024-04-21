//
// Created by alexl on 17.07.2023.
//
#if defined(BEE_COMPILE_WEBGPU)
#include "WebGPUGraphicsDevice.h"
#include "Core/DeletionQueue.h"
#include "WebGPUBufferPool.h"

namespace BeeEngine::Internal
{

    const WebGPUBuffer &WebGPUBufferPool::RequestBuffer(uint32_t size, WebGPUBufferUsage usage)
    {
        //TODO implement actual pool
        m_Buffers.push_back({CreateBuffer(size, usage), size, 0, usage, false});
        return m_Buffers.back();
    }

    void WebGPUBufferPool::ReleaseBuffer(const WebGPUBuffer &buffer)
    {
        if(buffer.m_IsDestroyed)
        {
            return;
        }

        auto found = std::find(m_Buffers.begin(), m_Buffers.end(), buffer);
        if(found == m_Buffers.end())
        {
            BeeCoreWarn("Tried to release buffer that was not allocated by this pool");
            return;
        }
        DestroyBuffer(*found);
        found->m_IsDestroyed = true;
        //std::remove(m_Buffers.begin(), m_Buffers.end(), buffer);
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
                if(buffer.m_IsDestroyed)
                {
                    continue;
                }
                DestroyBuffer(buffer);
            }
            m_Buffers.clear();
        });
    }

    WebGPUBufferPool::~WebGPUBufferPool()
    {

    }
}
#endif