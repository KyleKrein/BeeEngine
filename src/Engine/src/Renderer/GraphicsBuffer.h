//
// Created by alexl on 13.05.2023.
//

#pragma once
#include "Core/TypeDefines.h"

namespace BeeEngine
{
    class GraphicsBuffer
    {
    public:
        virtual ~GraphicsBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetData(const void* data, uint32_t size) = 0;

        virtual uint32_t GetSize() const = 0;

        static Ref<GraphicsBuffer> CreateVertexBuffer(uint32_t size);
        static Ref<GraphicsBuffer> CreateIndexBuffer(uint32_t size);
    };
}
