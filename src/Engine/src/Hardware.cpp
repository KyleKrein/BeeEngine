//
// Created by Aleksandr on 02.03.2024.
//

#include "Hardware.h"

#include "Platform/Vulkan/VulkanGraphicsDevice.h"
#include "Renderer/Renderer.h"

namespace BeeEngine
{
    bool Hardware::HasRayTracingSupport()
    {
        switch (Renderer::GetAPI()) {
            case WebGPU:
                return false;
#if defined(BEE_COMPILE_VULKAN)
            case Vulkan:
                return Internal::VulkanGraphicsDevice::GetInstance().HasRayTracingSupport();
#endif
            default:
                return false;
        }
    }

}
