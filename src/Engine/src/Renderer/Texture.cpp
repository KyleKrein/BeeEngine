//
// Created by alexl on 13.05.2023.
//

#include "Texture.h"
#include "Core/Logging/Log.h"
#include "Renderer.h"
#include "Platform/Vulkan/VulkanTexture2D.h"
#include "Platform/WebGPU/WebGPUTexture2D.h"

namespace BeeEngine
{

    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, gsl::span<std::byte> data, uint32_t numberOfChannels)
    {
        BEE_PROFILE_FUNCTION();
        switch (Renderer::GetAPI())
        {
#if defined(BEE_COMPILE_VULKAN)
            case RenderAPI::Vulkan:
                return CreateRef<Internal::VulkanTexture2D>(width, height, data, numberOfChannels);
#endif
            case RenderAPI::WebGPU:
                return CreateRef<Internal::WebGPUTexture2D>(width, height, data, numberOfChannels);
            default:
                BeeCoreError("Unknown RenderAPI");
                throw std::exception();
        }
    }
}
