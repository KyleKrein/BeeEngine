//
// Created by Александр Лебедев on 01.07.2023.
//

#include "Pipeline.h"
#include "Renderer.h"
#include "Platform/WebGPU/WebGPUPipeline.h"

namespace BeeEngine
{

    Ref<Pipeline> BeeEngine::Pipeline::Create(const Ref<ShaderModule>& vertexShader, const Ref<ShaderModule>& fragmentShader)
    {
        BeeExpects(vertexShader->GetType() == ShaderType::Vertex);
        BeeExpects(fragmentShader->GetType() == ShaderType::Fragment);
        switch (Renderer::GetAPI())
        {
            case WebGPU:
                return CreateRef<Internal::WebGPUPipeline>(vertexShader, fragmentShader);
            case Vulkan:
            case OpenGL:
            case Metal:
            case DirectX:
            case NotAvailable:
            default:
                BeeCoreError("Unknown renderer API");
                return nullptr;
        }
    }
}
