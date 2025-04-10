//
// Created by alexl on 13.05.2023.
//

#include "Texture.h"
#include "Core/AssetManagement/TextureImporter.h"
#include "Core/Logging/Log.h"
#include "Platform/Vulkan/VulkanTexture2D.h"
#include "Platform/WebGPU/WebGPUTexture2D.h"
#include "Renderer.h"

namespace BeeEngine
{

    Ref<Texture2D>
    Texture2D::Create(uint32_t width, uint32_t height, std::span<std::byte> data, uint32_t numberOfChannels)
    {
        return CreateRef<Texture2D>(
            CreateScope<Internal::VulkanGPUTextureResource>(width, height, data, numberOfChannels));
    }
    Scope<GPUTextureResource>
    GPUTextureResource::Create(uint32_t width, uint32_t height, std::span<std::byte> data, uint32_t numberOfChannels)
    {
        BEE_PROFILE_FUNCTION();
        switch (Renderer::GetAPI())
        {
#if defined(BEE_COMPILE_VULKAN)
            case RenderAPI::Vulkan:
                return CreateScope<Internal::VulkanGPUTextureResource>(width, height, data, numberOfChannels);
#endif
#if defined(BEE_COMPILE_WEBGPU)
            case RenderAPI::WebGPU:
                return CreateRef<Internal::WebGPUTexture2D>(width, height, data, numberOfChannels);
#endif

            default:
                BeeCoreError("Unknown RenderAPI");
                throw std::exception();
        }
    }
    Scope<GPUTextureResource> GPUTextureResource::Create(const Path& path)
    {
        return TextureImporter::LoadTextureFromFile(path);
    }
} // namespace BeeEngine
