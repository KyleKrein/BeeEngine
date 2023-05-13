//
// Created by alexl on 13.05.2023.
//

#include "OpenGLVertexArray.h"
#include "glad/glad.h"


namespace BeeEngine
{

    OpenGLVertexArray::OpenGLVertexArray(Ref<GraphicsBuffer> vertexBuffer, Ref<GraphicsBuffer> indexBuffer)
    {
        glGenVertexArrays(1, &m_RendererID);
        glBindVertexArray(m_RendererID);

        vertexBuffer->Bind();
        indexBuffer->Bind();
    }

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        glDeleteVertexArrays(1, &m_RendererID);
    }

    void OpenGLVertexArray::Bind() const
    {
        glBindVertexArray(m_RendererID);
    }

    void OpenGLVertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }
}
