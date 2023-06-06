//
// Created by alexl on 14.05.2023.
//

#include "OpenGLFrameBuffer.h"
#include "Core/Logging/Log.h"
#include "glad/glad.h"
#include "Core/Application.h"
#include "Debug/OpenGLDebug.h"
#include "Utils/Expects.h"


namespace BeeEngine::Internal
{

    OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferPreferences &preferences)
            : FrameBuffer(preferences)
    {
        BEE_PROFILE_FUNCTION();
        OpenGLFrameBuffer::Invalidate();
    }

    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        BEE_PROFILE_FUNCTION();
        if(glIsFramebuffer(m_RendererID) == GL_TRUE)
        {
            glDeleteFramebuffers(1, &m_RendererID);
        }
        if(glIsTexture(m_ColorAttachment) == GL_TRUE)
        {
            glDeleteTextures(1, &m_ColorAttachment);
        }
        if(glIsTexture(m_DepthAttachment) == GL_TRUE)
        {
            glDeleteTextures(1, &m_DepthAttachment);
        }
        //OPENGL_CHECK_ERRORS todo: this generates infinite errors 1282
    }

    void OpenGLFrameBuffer::Bind() const
    {
        BEE_PROFILE_FUNCTION();
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, m_Preferences.Width, m_Preferences.Height);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLFrameBuffer::Unbind() const
    {
        BEE_PROFILE_FUNCTION();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Application::GetInstance().GetWidth(), Application::GetInstance().GetHeight());
        OPENGL_CHECK_ERRORS
    }

    void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(width > 0 && height > 0 && width < 100000 && height < 100000);
        m_Preferences.Width = width;
        m_Preferences.Height = height;
        Invalidate();
    }

    void OpenGLFrameBuffer::Invalidate()
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(m_Preferences.Width > 0 && m_Preferences.Height > 0 && m_Preferences.Width < 100000 && m_Preferences.Height < 100000);
        if(m_RendererID)
        {
            glDeleteFramebuffers(1, &m_RendererID);
            glDeleteTextures(1, &m_ColorAttachment);
            glDeleteTextures(1, &m_DepthAttachment);
            OPENGL_CHECK_ERRORS
        }
        glGenFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glGenTextures(1, &m_ColorAttachment);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, gsl::narrow_cast<int>(m_Preferences.Width)
                , gsl::narrow_cast<int>(m_Preferences.Height),
                     0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

        glGenTextures(1, &m_DepthAttachment);
        glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, gsl::narrow_cast<int>(m_Preferences.Width),
                     gsl::narrow_cast<int>(m_Preferences.Height),
                     0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D,
                               m_DepthAttachment, 0); //возможно нужен GL_DEPTH_ATTACHMENT
        BeeCoreAssert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer creation failed!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        OPENGL_CHECK_ERRORS

        BeeEnsures(m_RendererID != 0 && m_ColorAttachment != 0 && m_DepthAttachment != 0);
    }
}
