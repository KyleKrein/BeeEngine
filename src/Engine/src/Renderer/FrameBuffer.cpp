//
// Created by alexl on 14.05.2023.
//

#include "FrameBuffer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"


namespace BeeEngine
{

    Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferPreferences &preferences)
    {
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                return CreateRef<OpenGLFrameBuffer>(preferences);
            default:
                BeeCoreError("Unknown RenderAPI");
                return nullptr;
        }
    }

    FrameBuffer::FrameBuffer(const FrameBufferPreferences &preferences)
            : m_Preferences(preferences)
    {

    }
}