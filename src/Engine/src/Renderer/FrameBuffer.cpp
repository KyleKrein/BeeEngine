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
            Expects(preferences.Width > 0 && preferences.Height > 0 && preferences.Width < 100000 && preferences.Height < 100000);
            case RenderAPI::OpenGL:
                return CreateRef<Internal::OpenGLFrameBuffer>(preferences);
            default:
                BeeCoreFatalError("Unknown RenderAPI");
        }
    }

    FrameBuffer::FrameBuffer(const FrameBufferPreferences &preferences)
            : m_Preferences(preferences), m_ColorAttachment(0), m_DepthAttachment(0), m_RendererID(0)
    {

    }
}
