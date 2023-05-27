//
// Created by alexl on 14.05.2023.
//

#include "FrameBuffer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"


namespace BeeEngine
{

    Scope<FrameBuffer> FrameBuffer::Create(const FrameBufferPreferences &preferences)
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(preferences.Width > 0 && preferences.Height > 0 && preferences.Width < 100000 && preferences.Height < 100000);
        switch (Renderer::GetAPI())
        {
            case RenderAPI::OpenGL:
                return CreateScope<Internal::OpenGLFrameBuffer>(preferences);
            default:
                BeeCoreFatalError("Unknown RenderAPI");
        }
    }

    Scope<FrameBuffer> FrameBuffer::Create(FrameBufferPreferences &&preferences)
    {
        return FrameBuffer::Create(preferences);
    }

    FrameBuffer::FrameBuffer(const FrameBufferPreferences &preferences)
            : m_Preferences(preferences), m_ColorAttachment(0), m_DepthAttachment(0), m_RendererID(0)
    {

    }


}
