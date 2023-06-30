//
// Created by Александр Лебедев on 30.06.2023.
//

#pragma once

#include "Renderer/CommandBuffer.h"
#include <webgpu/webgpu.h>
namespace BeeEngine::Internal
{
    class WebGPUCommandBuffer: public CommandBuffer
    {
    public:
        explicit WebGPUCommandBuffer(WGPUCommandEncoder encoder)
            : CommandBuffer(encoder)
        {}
        ~WebGPUCommandBuffer() override = default;
        [[nodiscard]] WGPUCommandEncoder GetHandle() const { return (WGPUCommandEncoder)m_Handle; }
    };
}
