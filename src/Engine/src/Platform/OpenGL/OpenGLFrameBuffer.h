//
// Created by alexl on 14.05.2023.
//

#pragma once

#include "Renderer/FrameBuffer.h"
#include "Utils/Expects.h"

namespace BeeEngine
{
    namespace Internal
    {
        class OpenGLFrameBuffer: public FrameBuffer
        {
        public:
            explicit OpenGLFrameBuffer(const FrameBufferPreferences& preferences);
            ~OpenGLFrameBuffer() override;
            uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override
            {
                BeeExpects(index < m_ColorAttachments.size());
                return m_ColorAttachments[index];
            }
            uint32_t GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }
            uint32_t GetRendererID() const override { return m_RendererID; }
            void Bind() const override;
            void Unbind() const override;
            void Resize(uint32_t width, uint32_t height) override;
            void Invalidate() override;

        private:
            std::vector<FrameBufferTextureSpecification> m_ColorAttachmentSpecification;
            FrameBufferTextureSpecification m_DepthAttachmentSpecification;
            FrameBufferPreferences m_Preferences;
            uint32_t m_RendererID;

            std::vector<uint32_t> m_ColorAttachments;
            uint32_t m_DepthAttachment;
        };
    }
}
