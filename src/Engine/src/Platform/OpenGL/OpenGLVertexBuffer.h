//
// Created by alexl on 13.05.2023.
//

#pragma once

#include "Renderer/GraphicsBuffer.h"

namespace BeeEngine::Internal
{
    class OpenGLVertexBuffer: public GraphicsBuffer
    {
    public:
        OpenGLVertexBuffer(uint32_t size);
        ~OpenGLVertexBuffer() override;

        void Bind() const override;
        void Unbind() const override;

        void SetData(gsl::span<std::byte> data) override;

        uint32_t GetSize() const override { return m_Size; }

    private:
        uint32_t m_RendererID;
        uint32_t m_Size;
    };
}
