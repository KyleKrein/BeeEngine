//
// Created by Александр Лебедев on 27.06.2023.
//

#include "Material.h"
#include "Platform/WebGPU/WebGPUMaterial.h"

namespace BeeEngine
{
    Ref<Material> Material::Create(const std::filesystem::path& vertexShader, const std::filesystem::path& fragmentShader, bool loadFromCache)
    {
        return CreateRef<Internal::WebGPUMaterial>(vertexShader, fragmentShader, loadFromCache);
    }
}