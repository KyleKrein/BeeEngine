//
// Created by alexl on 13.05.2023.
//

#pragma once

#include "Renderer/Texture.h"
#include "glad/glad.h"

namespace BeeEngine::Internal
{
    class OpenGLTexture2D: public Texture2D
    {
    public:
        OpenGLTexture2D(std::string_view path);
        OpenGLTexture2D(uint32_t width, uint32_t height);
        OpenGLTexture2D(gsl::span<std::byte> data);
        virtual ~OpenGLTexture2D() override;

        virtual void SetData(gsl::span<std::byte> data, uint32_t numberOfChannels) override;

        virtual void Bind(uint32_t slot = 0) const;
    private:
        GLenum m_InternalFormat;
    };
}
