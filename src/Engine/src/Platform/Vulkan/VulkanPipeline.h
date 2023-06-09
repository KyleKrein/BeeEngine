//
// Created by alexl on 09.06.2023.
//

#pragma once
#include "vulkan/vulkan.hpp"
#include "VulkanRenderPass.h"
#include "VulkanShaderModule.h"
#include "Core/TypeDefines.h"

namespace BeeEngine::Internal
{
    struct GraphicsPipelineInBundle
    {
        uint32_t ViewportWidth;
        uint32_t ViewportHeight;
        vk::Format SwapChainImageFormat;
        Ref<VulkanShaderModule> VertexShader;
        Ref<VulkanShaderModule> FragmentShader;
    };
    class VulkanPipeline
    {
    public:
        VulkanPipeline(const GraphicsPipelineInBundle& specification);
        ~VulkanPipeline();



    private:
        vk::Pipeline m_Pipeline;
        vk::Device& m_Device;
        vk::PipelineLayout m_PipelineLayout;
        VulkanRenderPass m_RenderPass;
        void CreateLayout();
    };
}
