//
// Created by alexl on 14.05.2023.
//

#include "FrameBuffer.h"
#include "Renderer.h"
#include "Platform/WebGPU/WebGPUFramebuffer.h"


namespace BeeEngine
{

    Scope<FrameBuffer> FrameBuffer::Create(const FrameBufferPreferences &preferences)
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(preferences.Width > 0 && preferences.Height > 0 && preferences.Width < 100000 && preferences.Height < 100000);
        switch (Renderer::GetAPI())
        {
            case RenderAPI::WebGPU:
                return CreateScope<Internal::WebGPUFrameBuffer>(preferences);
            default:
                BeeCoreFatalError("Unknown RenderAPI");
        }
    }

    Scope<FrameBuffer> FrameBuffer::Create(FrameBufferPreferences &&preferences)
    {
        return FrameBuffer::Create(preferences);
    }

    bool FrameBuffer::IsDepthFormat(FrameBufferTextureFormat format) const
    {
        switch (format)
        {
            case FrameBufferTextureFormat::Depth24:
                return true;
            default:
                return false;
        }
    }


}
