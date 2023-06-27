//
// Created by Александр Лебедев on 27.06.2023.
//

#include "Material.h"
#include "Platform/Vulkan/VulkanMaterial.h"

namespace BeeEngine
{
    Ref<Material> Material::Create()
    {
        return CreateRef<Internal::VulkanMaterial>();
    }
}