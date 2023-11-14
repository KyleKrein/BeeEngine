//
// Created by alexl on 14.05.2023.
//

#pragma once

#include "Renderer/FrameBuffer.h"
#include "Core/CodeSafety/Expects.h"

namespace BeeEngine
{
    namespace Internal
    {
        class OpenGLFrameBuffer: public FrameBuffer
        {
        public:
            explicit OpenGLFrameBuffer(const FrameBufferPreferences& preferences);
            ~OpenGLFrameBuffer() override;
            uintptr_t GetColorAttachmentRendererID(uint32_t index = 0) const override
            {
                BeeExpects(index < m_ColorAttachments.size());
                return m_ColorAttachments[index];
            }
            uintptr_t GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }
            virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) const override;
            void ClearColorAttachment(uint32_t attachmentIndex, int value) override;
            uint32_t GetRendererID() const override { return m_RendererID; }
            void Bind() override;
            void Unbind() override;
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
