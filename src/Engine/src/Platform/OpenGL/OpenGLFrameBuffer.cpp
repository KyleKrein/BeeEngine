//
// Created by alexl on 14.05.2023.
//

#include "OpenGLFrameBuffer.h"
#include "Core/Logging/Log.h"
#include "glad/glad.h"
#include "Core/Application.h"
#include "Debug/OpenGLDebug.h"
#include "Core/CodeSafety/Expects.h"


namespace BeeEngine::Internal
{
    static GLenum TextureTarget(bool multisampled)
    {
        return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    }
    static GLenum TextureFormatToGL(FrameBufferTextureFormat format)
    {
        switch (format)
        {
            case FrameBufferTextureFormat::RGBA8:
                return GL_RGBA8;
            case FrameBufferTextureFormat::RedInteger:
                return GL_RED_INTEGER;
            default:
                BeeCoreFatalError("Unknown TextureFormat");
        }
    }
    static GLenum TextureDataTypeToGL(FrameBufferTextureFormat format)
    {
        switch (format)
        {
            case FrameBufferTextureFormat::RGBA8:
                return GL_UNSIGNED_BYTE;
            case FrameBufferTextureFormat::RedInteger:
                return GL_INT;
            default:
            BeeCoreFatalError("Unknown TextureFormat");
        }
    }
    static void CreateTextures(bool multisampled, GLuint* outID, GLuint count)
    {
        glCreateTextures(TextureTarget(multisampled), count, outID);
    }
    static void BindTexture(bool multisampled, GLuint id)
    {
        glBindTexture(TextureTarget(multisampled), id);
    }
    static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
    {
        bool multisampled = samples > 1;
        if (multisampled)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //todo: this is not needed
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); //todo: this is not needed
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //todo: this is not needed
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //todo: this is not needed
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);

    }

    void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
    {
        bool multisampled = samples > 1;
        if (multisampled)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
        }
        else
        {
            glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //todo: this is not needed
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); //todo: this is not needed
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //todo: this is not needed
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //todo: this is not needed
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
    }

    OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferPreferences &preferences)
            : m_Preferences(preferences)
    {
        BEE_PROFILE_FUNCTION();
        for (auto specification: m_Preferences.Attachments.Attachments)
        {
            if(!IsDepthFormat(specification.TextureFormat))
            {
                m_ColorAttachmentSpecification.emplace_back(specification);
            }
            else
            {
                m_DepthAttachmentSpecification = specification;
            }
        }
        OpenGLFrameBuffer::Invalidate();
    }

    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        BEE_PROFILE_FUNCTION();
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
        glDeleteTextures(1, &m_DepthAttachment);
        //OPENGL_CHECK_ERRORS todo: this generates infinite errors 1282
    }

    void OpenGLFrameBuffer::Bind()
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

            glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
            glDeleteTextures(1, &m_DepthAttachment);

            m_ColorAttachments.clear();
            m_DepthAttachment = 0;
            OPENGL_CHECK_ERRORS
        }
        glCreateFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        const bool multisample = m_Preferences.Samples > 1;

        //Attachments
        if(!m_ColorAttachmentSpecification.empty())
        {
            m_ColorAttachments.resize(m_ColorAttachmentSpecification.size());
            Internal::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());
            for (int i = 0; i < m_ColorAttachments.size(); ++i)
            {
                BindTexture(multisample, m_ColorAttachments[i]);
                switch (m_ColorAttachmentSpecification[i].TextureFormat)
                {
                    case FrameBufferTextureFormat::RGBA8:
                        AttachColorTexture(m_ColorAttachments[i], m_Preferences.Samples, GL_RGBA8, GL_RGBA, m_Preferences.Width, m_Preferences.Height, i);
                        break;
                    case FrameBufferTextureFormat::RedInteger:
                        AttachColorTexture(m_ColorAttachments[i], m_Preferences.Samples, GL_R32I, GL_RED_INTEGER, m_Preferences.Width, m_Preferences.Height, i);
                        break;
                    default:
                        BeeCoreError("Unknown texture format");
                }
            }
        }

        if(m_DepthAttachmentSpecification.TextureFormat != FrameBufferTextureFormat::None)
        {
            Internal::CreateTextures(multisample, &m_DepthAttachment, 1);
            BindTexture(multisample, m_DepthAttachment);
            switch (m_DepthAttachmentSpecification.TextureFormat)
            {
                case FrameBufferTextureFormat::Depth24:
                    AttachDepthTexture(m_DepthAttachment, m_Preferences.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Preferences.Width, m_Preferences.Height);
                    break;
                default:
                    BeeExpects(false);
            }
        }

        if (m_ColorAttachments.size() > 1)
        {
            BeeCoreAssert(m_ColorAttachments.size() <= 4, "Only 4 color attachments are supported");
            GLenum buffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
            glDrawBuffers(m_ColorAttachments.size(), buffers);
        }
        else if(m_ColorAttachments.empty())
        {
            glDrawBuffer(GL_NONE);
        }

        BeeCoreAssert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer creation failed!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        OPENGL_CHECK_ERRORS

        //BeeEnsures(m_RendererID != 0 && m_ColorAttachment != 0 && m_DepthAttachment != 0);
    }

    int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y) const
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(attachmentIndex < m_ColorAttachments.size());
        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
        int pixelData;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
        return pixelData;
    }

    void OpenGLFrameBuffer::ClearColorAttachment(uint32_t attachmentIndex, int value)
    {
        BeeExpects(attachmentIndex < m_ColorAttachments.size());

        const auto &spec = m_ColorAttachmentSpecification[attachmentIndex];

        glClearTexImage(m_ColorAttachments[attachmentIndex], 0, TextureFormatToGL(spec.TextureFormat), TextureDataTypeToGL(spec.TextureFormat), &value);
    }
}
