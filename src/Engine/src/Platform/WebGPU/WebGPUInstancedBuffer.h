//
// Created by alexl on 12.07.2023.
//

#pragma once

#include "Renderer/InstancedBuffer.h"

namespace BeeEngine::Internal
{
    class WebGPUInstancedBuffer: public InstancedBuffer
    {
    public:
        WebGPUInstancedBuffer(in<WGPUVertexBufferLayout> shaderModule, size_t maxInstances);
        ~WebGPUInstancedBuffer() override;
        WebGPUInstancedBuffer(const WebGPUInstancedBuffer& other) = delete;
        WebGPUInstancedBuffer& operator=(const WebGPUInstancedBuffer& other ) = delete;
        void SetData(void* data, size_t size) override;
        void Bind(void* cmd) override;
    private:
        class WebGPUGraphicsDevice& m_GraphicsDevice;
        WGPUBuffer m_Buffer;
        size_t m_Size;
        size_t m_MaxInstances;
        size_t m_OneInstanceSize;
    };
}
