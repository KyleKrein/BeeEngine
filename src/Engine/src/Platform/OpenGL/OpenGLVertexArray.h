//
// Created by alexl on 13.05.2023.
//

#pragma once

#include "Renderer/GraphicsBuffer.h"
#include "Renderer/VertexArray.h"

namespace BeeEngine
{
    class OpenGLVertexArray: public VertexArray
    {
    public:
        OpenGLVertexArray(Ref<GraphicsBuffer> vertexBuffer, Ref<GraphicsBuffer> indexBuffer);
        ~OpenGLVertexArray() override;

        void Bind() const override;
        void Unbind() const override;
    private:
        uint32_t m_RendererID;
    };
}
