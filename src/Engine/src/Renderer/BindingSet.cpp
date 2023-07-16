//
// Created by alexl on 15.07.2023.
//

#include "BindingSet.h"
#include "IBindable.h"
#include "Renderer/Renderer.h"
#include "Platform/WebGPU/WebGPUBindingSet.h"

namespace BeeEngine
{

    Ref<BindingSet> BindingSet::Create(std::initializer_list<BindingSetElement> elements)
    {
        switch (Renderer::GetAPI())
        {
            case WebGPU:
                return CreateRef<Internal::WebGPUBindingSet>(elements);
            case OpenGL:
            case Metal:
            case DirectX:
            case Vulkan:
            case NotAvailable:
                BeeCoreError("BindingSet::Create: API not available!");
                return nullptr;
        }
        return nullptr;
    }
}
