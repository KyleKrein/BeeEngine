//
// Created by alexl on 14.05.2023.
//

#include "OpenGLFrameBuffer.h"
#include "Core/Logging/Log.h"
#include "glad/glad.h"
#include "Core/Application.h"


namespace BeeEngine
{

    OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferPreferences &preferences)
            : FrameBuffer(preferences)
    {
        OpenGLFrameBuffer::Invalidate();
    }

    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(1, &m_ColorAttachment);
        glDeleteTextures(1, &m_DepthAttachment);
    }

    void OpenGLFrameBuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, m_Preferences.Width, m_Preferences.Height);
    }

    void OpenGLFrameBuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Application::GetInstance()->GetWidth(), Application::GetInstance()->GetHeight());
    }

    void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
    {
        m_Preferences.Width = width;
        m_Preferences.Height = height;
        Invalidate();
    }

    void OpenGLFrameBuffer::Invalidate()
    {
        if (m_Preferences.Width == 0 || m_Preferences.Height == 0)
        {
            BeeCoreWarn("Invalid width {0} or height {1} specified for FrameBuffer", m_Preferences.Width, m_Preferences.Height);
            return;
        }
        if(m_RendererID)
        {
            glDeleteFramebuffers(1, &m_RendererID);
            glDeleteTextures(1, &m_ColorAttachment);
            glDeleteTextures(1, &m_DepthAttachment);
        }
        glGenFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glGenTextures(1, &m_ColorAttachment);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_Preferences.Width, m_Preferences.Height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

        glGenTextures(1, &m_DepthAttachment);
        glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Preferences.Width, m_Preferences.Height,
                     0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D,
                               m_DepthAttachment, 0); //возможно нужен GL_DEPTH_ATTACHMENT
        BeeCoreAssert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer creation failed!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
