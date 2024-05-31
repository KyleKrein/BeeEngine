//
// Created by Aleksandr on 25.02.2024.
//
#include "TopLevelAccelerationStructure.h"

#include "Core/Application.h"
#include "Platform/Vulkan/VulkanTLAS.h"
#include "Renderer.h"

namespace BeeEngine
{
    Ref<TopLevelAccelerationStructure> TopLevelAccelerationStructure::Create()
    {
        switch (Renderer::GetAPI())
        {
            case WebGPU:
                BeeCoreError("WebGPU does not support TopLevelAccelerationStructure");
                return nullptr;
#if defined(BEE_COMPILE_VULKAN)
            case Vulkan:
                return CreateRef<Internal::VulkanTLAS>();
#endif
            case NotAvailable:
                BeeCoreError("Renderer API is not available");
                break;
        }
        return nullptr;
    }
} // namespace BeeEngine
