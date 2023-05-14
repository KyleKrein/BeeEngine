//
// Created by alexl on 13.05.2023.
//

#pragma once
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

        virtual void SetData(const void* data, uint32_t size) = 0;

        void SetLayout(const BufferLayout& layout)
        {
            m_Layout = layout;
        };

        inline const BufferLayout& GetLayout() const
        {
            return m_Layout;
        };

        virtual uint32_t GetSize() const = 0;

        static Ref<GraphicsBuffer> CreateVertexBuffer(uint32_t size);
        static Ref<GraphicsBuffer> CreateIndexBuffer(uint32_t data[], uint32_t size);
protected:
        BufferLayout m_Layout;
    };
}
