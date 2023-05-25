//
// Created by alexl on 13.05.2023.
//

#pragma once

#include "Renderer/GraphicsBuffer.h"

namespace BeeEngine::Internal
{
    class OpenGLIndexBuffer: public GraphicsBuffer
    {
    public:
        OpenGLIndexBuffer(uint32_t data[], uint32_t size);
        ~OpenGLIndexBuffer() override;

        void Bind() const override;
        void Unbind() const override;

        void SetData(const void* data, uint32_t size) override;

        uint32_t GetSize() const override { return m_Size; }

    private:
        uint32_t m_RendererID;
        uint32_t m_Size;
    };
}
