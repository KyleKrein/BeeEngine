//
// Created by alexl on 15.07.2023.
//

#include "BindingSet.h"
#include "Core/TypeDefines.h"
#include "IBindable.h"
#include "Platform/Vulkan/VulkanBindingSet.h"
#include "Platform/WebGPU/WebGPUBindingSet.h"
#include "Renderer/Renderer.h"

namespace BeeEngine
{

    Ref<BindingSet> BindingSet::Create(std::initializer_list<BindingSetElement> elements)
    {
        switch (Renderer::GetAPI())
        {
#if defined(BEE_COMPILE_WEBGPU)
            case WebGPU:
                return CreateRef<Internal::WebGPUBindingSet>(elements);
#endif
#if defined(BEE_COMPILE_VULKAN)
            case Vulkan:
                return CreateRef<Internal::VulkanBindingSet>(elements);
#endif
            default:
                BeeCoreError("BindingSet::Create: API not available!");
                return nullptr;
        }
        return nullptr;
    }

    FrameScope<BindingSet> BindingSet::CreateFrameScope(std::initializer_list<BindingSetElement> elements)
    {
        switch (Renderer::GetAPI())
        {
#if defined(BEE_COMPILE_WEBGPU)
            case WebGPU:
                // return CreateFrameScope<Internal::WebGPUBindingSet>(elements);
                return FramePtr<Internal::WebGPUBindingSet>(new Internal::WebGPUBindingSet(elements));
#endif
#if defined(BEE_COMPILE_VULKAN)
            case Vulkan:
                return FramePtr<Internal::VulkanBindingSet>(new Internal::VulkanBindingSet(elements));
#endif
            default:
                BeeCoreError("BindingSet::Create: API not available!");
                return nullptr;
        }
        return nullptr;
    }
} // namespace BeeEngine
