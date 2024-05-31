//
// Created by Александр Лебедев on 27.06.2023.
//

#include "Material.h"

#include "Core/Application.h"
#include "Core/AssetManagement/AssetManager.h"
#include "MaterialData.h"
#include "Platform/Vulkan/VulkanMaterial.h"
#include "Platform/WebGPU/WebGPUMaterial.h"
#include "Renderer.h"

namespace BeeEngine
{
    Ref<Material> Material::Create(const std::filesystem::path& vertexShader,
                                   const std::filesystem::path& fragmentShader,
                                   bool loadFromCache)
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
    static String defaultLocale = "en_En";
    Texture2D* MaterialInstance::GetColorTexture() const
    {
        if (!AssetManager::IsAssetHandleValid(colorTexture))
            return &Application::GetInstance().GetAssetManager().GetTexture("Blank");
        return &AssetManager::GetAsset<Texture2D>(colorTexture, defaultLocale);
    }
    Texture2D* MaterialInstance::GetMetalRoughTexture() const
    {
        if (!AssetManager::IsAssetHandleValid(metalRoughTexture))
            return &Application::GetInstance().GetAssetManager().GetTexture("Blank");
        return &AssetManager::GetAsset<Texture2D>(metalRoughTexture, defaultLocale);
    }

} // namespace BeeEngine