//
// Created by alexl on 11.07.2023.
//

#include "UniformBuffer.h"
#include "Platform/WebGPU/WebGPUUniformBuffer.h"


namespace BeeEngine
{
    Ref<UniformBuffer> UniformBuffer::Create(size_t size)
    {
        return CreateRef<Internal::WebGPUUniformBuffer>(size);
    }
}
