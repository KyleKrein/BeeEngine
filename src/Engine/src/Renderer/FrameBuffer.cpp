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
        BEE_PROFILE_FUNCTION();
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                return CreateRef<Internal::OpenGLFrameBuffer>(preferences);
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
