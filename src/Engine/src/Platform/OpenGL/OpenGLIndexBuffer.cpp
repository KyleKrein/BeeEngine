//
// Created by alexl on 13.05.2023.
//

#include "OpenGLIndexBuffer.h"
#include "glad/glad.h"
#include "Core/Logging/Log.h"
#include "Debug/OpenGLDebug.h"
#include "Debug/Instrumentor.h"


namespace BeeEngine
{

    OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t data[], uint32_t size)
    {
        BEE_PROFILE_FUNCTION();
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        OPENGL_CHECK_ERRORS
        m_Size = size;
        OpenGLIndexBuffer::SetData(data, size);
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer()
    {
        BEE_PROFILE_FUNCTION();
        glDeleteBuffers(1, &m_RendererID);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLIndexBuffer::Bind() const
    {
        BEE_PROFILE_FUNCTION();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLIndexBuffer::Unbind() const
    {
        BEE_PROFILE_FUNCTION();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLIndexBuffer::SetData(const void *data, uint32_t size)
    {
        BEE_PROFILE_FUNCTION();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data);
        OPENGL_CHECK_ERRORS
    }
}
