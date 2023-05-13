//
// Created by alexl on 13.05.2023.
//

#pragma once
#include "Core/TypeDefines.h"

namespace BeeEngine
{
    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetData(const void* data, uint32_t size) = 0;

        virtual uint32_t GetSize() const = 0;

        static Ref<VertexBuffer> Create(uint32_t size);
        static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
    };
}
