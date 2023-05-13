//
// Created by alexl on 13.05.2023.
//

#pragma once

#include "Renderer/Texture.h"

namespace BeeEngine
{
    class OpenGLTexture2D: public Texture2D
    {
    public:
        OpenGLTexture2D(const String& path);
        OpenGLTexture2D(uint32_t width, uint32_t height);
        virtual ~OpenGLTexture2D() override;

        virtual void SetData(void* data, uint32_t size) override;

        virtual void Bind(uint32_t slot = 0) const override;
    };
}
