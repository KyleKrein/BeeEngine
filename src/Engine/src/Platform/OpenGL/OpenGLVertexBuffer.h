//
// Created by alexl on 13.05.2023.
//

#pragma once

#include "Renderer/GraphicsBuffer.h"

namespace BeeEngine
{
    class OpenGLVertexBuffer: public GraphicsBuffer
    {
    public:
        OpenGLVertexBuffer(uint32_t size);
        ~OpenGLVertexBuffer() override;

        void Bind() const override;
        void Unbind() const override;

        void SetData(const void* data, uint32_t size) override;

        uint32_t GetSize() const override { return m_Size; }

    private:
        uint32_t m_RendererID;
        uint32_t m_Size;
    };
}
