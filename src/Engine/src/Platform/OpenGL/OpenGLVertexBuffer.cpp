//
// Created by alexl on 13.05.2023.
//

#include "OpenGLVertexBuffer.h"
#include "glad/glad.h"
#include "Core/Logging/Log.h"
#include "Debug/OpenGLDebug.h"
#include "Debug/Instrumentor.h"
#include "Core/CodeSafety/Expects.h"


namespace BeeEngine::Internal
{
    void OpenGLVertexBuffer::Bind() const
    {
        BEE_PROFILE_FUNCTION();
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLVertexBuffer::Unbind() const
    {
        BEE_PROFILE_FUNCTION();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLVertexBuffer::SetData(gsl::span<std::byte> data)
    {
        BeeExpects(!data.empty() && data.size() <= m_Size);
        BEE_PROFILE_FUNCTION();
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, gsl::narrow_cast<GLsizeiptr>(data.size()), data.data());
        OPENGL_CHECK_ERRORS
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer()
    {
        BEE_PROFILE_FUNCTION();
        glDeleteBuffers(1, &m_RendererID);
        OPENGL_CHECK_ERRORS
    }

    OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
    : m_RendererID(0), m_Size(size)
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(size > 0);
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        OPENGL_CHECK_ERRORS
    }
}


