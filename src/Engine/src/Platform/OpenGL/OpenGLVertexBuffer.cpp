//
// Created by alexl on 13.05.2023.
//

#include "OpenGLVertexBuffer.h"
#include "glad/glad.h"
#include "Core/Logging/Log.h"
#include "Debug/OpenGLDebug.h"


namespace BeeEngine
{
    void OpenGLVertexBuffer::Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLVertexBuffer::Unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLVertexBuffer::SetData(const void *data, uint32_t size)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
        OPENGL_CHECK_ERRORS
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer()
    {
        glDeleteBuffers(1, &m_RendererID);
        OPENGL_CHECK_ERRORS
    }

    OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
    {
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        OPENGL_CHECK_ERRORS
    }
}


