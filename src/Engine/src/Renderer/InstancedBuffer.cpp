//
// Created by alexl on 17.07.2023.
//
#include "InstancedBuffer.h"
#include "Platform/WebGPU/WebGPUInstancedBuffer.h"

BeeEngine::Scope<BeeEngine::InstancedBuffer> BeeEngine::InstancedBuffer::Create(size_t size)
{
    return BeeEngine::CreateScope<BeeEngine::Internal::WebGPUInstancedBuffer>(size);
}
