//
// Created by Александр Лебедев on 27.06.2023.
//

#pragma once

#include "Renderer/Material.h"
#include "vulkan/vulkan.h"

namespace BeeEngine::Internal
{
    class VulkanMaterial: public Material
    {
    public:
        VulkanMaterial() = default;
        VulkanMaterial(VkPipeline pipeline, VkPipelineLayout pipelineLayout): Pipeline(pipeline), PipelineLayout(pipelineLayout) {};
        VkPipeline Pipeline;
        VkPipelineLayout PipelineLayout;
    private:
    };
}

