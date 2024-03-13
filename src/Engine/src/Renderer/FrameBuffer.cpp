//
// Created by alexl on 14.05.2023.
//

#include "FrameBuffer.h"
#include "Renderer.h"
#include "Platform/Vulkan/VulkanFrameBuffer.h"
#include "Platform/WebGPU/WebGPUFramebuffer.h"


namespace BeeEngine
{

    Scope<FrameBuffer> FrameBuffer::Create(const FrameBufferPreferences &preferences)
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(preferences.Width > 0 && preferences.Height > 0 &&
            preferences.Width < std::numeric_limits<uint32_t>::max() &&
            preferences.Height < std::numeric_limits<uint32_t>::max());
        switch (Renderer::GetAPI())
        {
            case RenderAPI::WebGPU:
                return CreateScope<Internal::WebGPUFrameBuffer>(preferences);
#if defined(BEE_COMPILE_VULKAN)
            case RenderAPI::Vulkan:
                return CreateScope<Internal::VulkanFrameBuffer>(preferences);
#endif
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
