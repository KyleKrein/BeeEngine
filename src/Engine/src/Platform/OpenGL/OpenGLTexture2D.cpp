//
// Created by alexl on 13.05.2023.
//

#include "OpenGLTexture2D.h"
#include "Core/ResourceManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Core/Logging/Log.h"
#include "Debug/OpenGLDebug.h"


namespace BeeEngine
{

    OpenGLTexture2D::OpenGLTexture2D(const String &path)
    {
        BEE_PROFILE_FUNCTION();
        String filepath = ResourceManager::ProcessFilePath(path);

        stbi_set_flip_vertically_on_load(1);

        int width, height, channels;
        stbi_uc* data = nullptr;
        {
            data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
        }
        if (!data)
        {
            BeeCoreError("Failed to load image at filepath {0}!", filepath);
            return;
        }

        m_Width = width;
        m_Height = height;

        GLenum internalFormat = 0, dataFormat = 0;
        if (channels == 4)
        {
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        }
        else if (channels == 3)
        {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }

        m_InternalFormat = internalFormat;

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, 0);
        OPENGL_CHECK_ERRORS
    }

    OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
    {
        BEE_PROFILE_FUNCTION();
        m_Width = width;
        m_Height = height;

        m_InternalFormat = GL_RGBA8;

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, 0);
        OPENGL_CHECK_ERRORS
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        BEE_PROFILE_FUNCTION();
        glDeleteTextures(1, &m_RendererID);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLTexture2D::SetData(void *data, uint32_t size)
    {
        BEE_PROFILE_FUNCTION();
        uint32_t bpp = m_InternalFormat == GL_RGBA8 ? 4 : 3;
        BeeCoreAssert(size == m_Width * m_Height * bpp, "Data must be entire texture!");
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        OPENGL_CHECK_ERRORS
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const
    {
        BEE_PROFILE_FUNCTION();
        BeeCoreAssert(slot < 32, "Texture slot out of range!");
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        OPENGL_CHECK_ERRORS
    }


}
