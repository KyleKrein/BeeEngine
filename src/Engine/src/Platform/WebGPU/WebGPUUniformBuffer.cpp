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

    std::vector<IBindable::BindGroupLayoutEntryType> WebGPUUniformBuffer::GetBindGroupLayoutEntry() const
    {
        WGPUBindGroupLayoutEntry entry = {};
        m_GraphicsDevice.SetDefault(entry);
        entry.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
        entry.buffer.type = WGPUBufferBindingType_Uniform;
        return {entry};
    }

    std::vector<IBindable::BindGroupEntryType> WebGPUUniformBuffer::GetBindGroupEntry() const
    {
        WGPUBindGroupEntry entry = {};
        m_GraphicsDevice.SetDefault(entry);
        entry.buffer = m_Buffer;
        entry.size = m_Size;
        return {entry};
    }
}