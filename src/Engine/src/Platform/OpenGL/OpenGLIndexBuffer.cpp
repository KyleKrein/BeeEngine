//
// Created by alexl on 13.05.2023.
//

#include "OpenGLIndexBuffer.h"
#include "glad/glad.h"
#include "Core/Logging/Log.h"
#include "Debug/OpenGLDebug.h"
#include "Debug/Instrumentor.h"


namespace BeeEngine::Internal
{

    OpenGLIndexBuffer::OpenGLIndexBuffer(gsl::span<std::byte> data)
    {
        BEE_PROFILE_FUNCTION();
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size(), nullptr, GL_DYNAMIC_DRAW);
        OPENGL_CHECK_ERRORS
        m_Size = data.size();
        OpenGLIndexBuffer::SetData(data);
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

    void OpenGLIndexBuffer::SetData(gsl::span<std::byte> data)
    {
        BEE_PROFILE_FUNCTION();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, gsl::narrow_cast<GLsizeiptr>(data.size()), data.data());
        OPENGL_CHECK_ERRORS
    }
}
