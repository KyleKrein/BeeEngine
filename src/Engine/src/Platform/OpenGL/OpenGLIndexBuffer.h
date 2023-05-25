//
// Created by alexl on 13.05.2023.
//

#pragma once

#include "Renderer/GraphicsBuffer.h"
#include "gsl/gsl"

namespace BeeEngine::Internal
{
    class OpenGLIndexBuffer: public GraphicsBuffer
    {
    public:
        OpenGLIndexBuffer(gsl::span<std::byte> data);
        ~OpenGLIndexBuffer() override;

        void Bind() const override;
        void Unbind() const override;

        void SetData(gsl::span<std::byte> data) override;

        uint32_t GetSize() const override { return m_Size; }

    private:
        uint32_t m_RendererID;
        uint32_t m_Size;
    };
}
