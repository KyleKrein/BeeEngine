//
// Created by Александр Лебедев on 01.07.2023.
//

#include "Pipeline.h"
#include "Renderer.h"
#include "Platform/Vulkan/VulkanComputePipeline.h"
#include "Platform/Vulkan/VulkanPipeline.h"
#include "Platform/WebGPU/WebGPUPipeline.h"

namespace BeeEngine
{

    Ref<Pipeline> BeeEngine::Pipeline::Create(const Ref<ShaderModule>& vertexShader, const Ref<ShaderModule>& fragmentShader)
    {
        BeeExpects(vertexShader->GetType() == ShaderType::Vertex);
        BeeExpects(fragmentShader->GetType() == ShaderType::Fragment);
        switch (Renderer::GetAPI())
        {
#if defined(BEE_COMPILE_WEBGPU)
            case WebGPU:
                return CreateRef<Internal::WebGPUPipeline>(vertexShader, fragmentShader);
#endif

#if defined(BEE_COMPILE_VULKAN)
            case Vulkan:
                return CreateRef<Internal::VulkanPipeline>(vertexShader, fragmentShader);
#endif
            case NotAvailable:
            default:
                BeeCoreError("Unknown renderer API");
                return nullptr;
        }
    }

    Ref<Pipeline> Pipeline::Create(const Ref<ShaderModule>& computeShader)
    {
        BeeExpects(computeShader->GetType() == ShaderType::Compute);
        switch (Renderer::GetAPI())
        {
#if defined(BEE_COMPILE_VULKAN)
            case Vulkan:
                return CreateRef<Internal::VulkanComputePipeline>(computeShader);
#endif
            case NotAvailable:
                default:
                    BeeCoreError("Unknown renderer API");
            return nullptr;
        }
    }
}
