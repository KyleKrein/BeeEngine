//
// Created by alexl on 13.05.2023.
//

#pragma once

#include "GraphicsBuffer.h"

namespace BeeEngine
{
    class VertexArray
    {
    public:
        VertexArray() = default;
        static Ref<VertexArray> Create(Ref<GraphicsBuffer> vertexBuffer, Ref<GraphicsBuffer> indexBuffer);
        virtual ~VertexArray() = 0;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
    };
}
