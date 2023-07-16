//
// Created by alexl on 11.07.2023.
//

#pragma once

#include "Renderer/UniformBuffer.h"

namespace BeeEngine::Internal
{
    class WebGPUUniformBuffer: public UniformBuffer
    {
    public:
        WebGPUUniformBuffer(size_t size);
        ~WebGPUUniformBuffer() override;
        WebGPUUniformBuffer(const WebGPUUniformBuffer& other) = delete;
        WebGPUUniformBuffer& operator=(const WebGPUUniformBuffer& other ) = delete;
        void SetData(void* data, size_t size) override;
        void Bind(uint32_t binding) override;
        std::vector<WGPUBindGroupLayoutEntry> GetBindGroupLayoutEntry() const override;
        std::vector<WGPUBindGroupEntry> GetBindGroupEntry() const override;
    private:
        WGPUBuffer m_Buffer;
        size_t m_Size;
        class WebGPUGraphicsDevice& m_GraphicsDevice;
    };
}
