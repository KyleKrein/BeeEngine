//
// Created by alexl on 12.07.2023.
//

#pragma once

#include "Core/TypeDefines.h"

namespace BeeEngine
{
    class InstancedBuffer
    {
    public:
        InstancedBuffer() = default;
        virtual ~InstancedBuffer() = default;
        InstancedBuffer(const InstancedBuffer& other) = delete;
        InstancedBuffer& operator=(const InstancedBuffer& other ) = delete;
        virtual void SetData(void* data, size_t size) = 0;
        virtual void Bind(void* cmd) = 0;
    };
}