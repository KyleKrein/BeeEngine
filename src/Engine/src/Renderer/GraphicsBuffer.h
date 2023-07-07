//
// Created by alexl on 13.05.2023.
//

#pragma once

#include <gsl/span>
#include "Core/TypeDefines.h"
#include "BufferLayout.h"

namespace BeeEngine
{
    class GraphicsBuffer
    {
    public:
        GraphicsBuffer();

        virtual ~GraphicsBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetData(gsl::span<std::byte> data) = 0;

        virtual void SetLayout(const BufferLayout& layout) = 0;

        [[nodiscard]] virtual inline const BufferLayout& GetLayout() const = 0;

        [[nodiscard]] virtual uint32_t GetSize() const = 0;

        static Ref<GraphicsBuffer> CreateVertexBuffer(uint32_t size);
        static Ref<GraphicsBuffer> CreateVertexBuffer(uint32_t numberOfVertices, in<BufferLayout> layout);
        static Ref<GraphicsBuffer> CreateIndexBuffer(gsl::span<uint32_t> data);
    };
}
