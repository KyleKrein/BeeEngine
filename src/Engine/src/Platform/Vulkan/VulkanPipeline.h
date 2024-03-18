//
// Created by Aleksandr on 09.03.2024.
//

#pragma once
#include "Renderer/Pipeline.h"
#include <vulkan/vulkan.hpp>

#include "VulkanShaderModule.h"

namespace BeeEngine::Internal
{
    class VulkanPipeline final: public Pipeline
    {
    public:
        VulkanPipeline(const Ref<ShaderModule>& vertexShader, const Ref<ShaderModule>& fragmentShader);
        void Bind(CommandBuffer& commandBuffer) override;
        PipelineType GetType() const override { return PipelineType::Graphics; }

        ~VulkanPipeline() override;
        vk::PipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }
        static VulkanPipeline& GetCurrentPipeline() { return *s_CurrentPipeline; }
    private:
        VulkanGraphicsDevice& m_Device;
        Ref<VulkanShaderModule> m_VertexShader;
        Ref<VulkanShaderModule> m_FragmentShader;
        vk::Pipeline m_Pipeline;
        vk::PipelineLayout m_PipelineLayout;
        static VulkanPipeline* s_CurrentPipeline;
    };
}
