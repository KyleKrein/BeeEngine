//
// Created by alexl on 17.07.2023.
//

#pragma once
#include "Core/TypeDefines.h"
namespace BeeEngine
{
    struct RendererStatistics
    {
        size_t TotalInstanceCount{0};
        size_t TransparentInstanceCount{0};
        size_t OpaqueInstanceCount{0};
        size_t DrawCallCount{0};
        size_t VertexCount{0};
        size_t IndexCount{0};
        size_t AllocatedGPUMemory{0};
        size_t AllocatedCPUMemory{0};
        size_t AllocatedGPUBuffers{0};
    };
} // namespace BeeEngine