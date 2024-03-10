//
// Created by Александр Лебедев on 27.06.2023.
//

#include "Material.h"

#include "Renderer.h"
#include "Platform/Vulkan/VulkanMaterial.h"
#include "Platform/WebGPU/WebGPUMaterial.h"

namespace BeeEngine
{
    Ref<Material> Material::Create(const std::filesystem::path& vertexShader, const std::filesystem::path& fragmentShader, bool loadFromCache)
    {
        switch (Renderer::GetAPI())
        {
            case WebGPU:
                return CreateRef<Internal::WebGPUMaterial>(vertexShader, fragmentShader, loadFromCache);
#if defined(BEE_COMPILE_VULKAN)
            case Vulkan:
                return CreateRef<Internal::VulkanMaterial>(vertexShader, fragmentShader, loadFromCache);
#endif
            default:
                BeeCoreError("Unknown RendererAPI");
                return nullptr;
        }
    }
}