//
// Created by alexl on 14.05.2023.
//

#pragma once

#include "Renderer/FrameBuffer.h"

namespace BeeEngine
{
    namespace Internal
    {
        class OpenGLFrameBuffer: public FrameBuffer
        {
        public:
            explicit OpenGLFrameBuffer(const FrameBufferPreferences& preferences);
            ~OpenGLFrameBuffer() override;
            void Bind() const override;
            void Unbind() const override;
            void Resize(uint32_t width, uint32_t height) override;
            void Invalidate() override;
        };
    }
}
