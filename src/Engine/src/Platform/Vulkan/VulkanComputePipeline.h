//
// Created by Aleksandr on 01.04.2024.
//

#pragma once
#include "VulkanGraphicsDevice.h"
#include "VulkanShaderModule.h"
#include "Renderer/Pipeline.h"

namespace BeeEngine::Internal
{
    class VulkanComputePipeline final: public Pipeline
    {
    public:
        VulkanComputePipeline(const Ref<ShaderModule>& computeShader);
        PipelineType GetType() const override
        {
            return PipelineType::Compute;
        }

        void Bind(CommandBuffer& commandBuffer) override;

        ~VulkanComputePipeline() override;
    private:
        VulkanGraphicsDevice& m_Device;
        Ref<VulkanShaderModule> m_ComputeShader;
        vk::PipelineLayout m_PipelineLayout;
        vk::Pipeline m_Pipeline;
    };
}
