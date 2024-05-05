//
// Created by Aleksandr on 01.04.2024.
//

#include "VulkanComputePipeline.h"

#include "Renderer/CommandBuffer.h"
#include "VulkanShaderModule.h"

namespace BeeEngine::Internal
{
    VulkanComputePipeline::VulkanComputePipeline(const Ref<ShaderModule>& computeShader)
        : m_Device(VulkanGraphicsDevice::GetInstance()),
          m_ComputeShader(std::move(std::static_pointer_cast<VulkanShaderModule>(computeShader)))
    {
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};

        auto& descriptorSetLayouts = m_ComputeShader->GetDescriptorSetLayouts();
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        m_PipelineLayout = m_Device.GetDevice().createPipelineLayout(pipelineLayoutInfo);

        vk::PipelineShaderStageCreateInfo computeShaderStageInfo{};
        computeShaderStageInfo.stage = vk::ShaderStageFlagBits::eCompute;
        computeShaderStageInfo.module = m_ComputeShader->GetShaderModule();
        computeShaderStageInfo.pName = "main";

        vk::ComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.stage = computeShaderStageInfo;
        pipelineInfo.layout = m_PipelineLayout;

        auto result = m_Device.GetDevice().createComputePipeline(nullptr, pipelineInfo);
        if (result.result != vk::Result::eSuccess)
        {
            BeeCoreError("Failed to create compute pipeline");
        }
        m_Pipeline = result.value;
    }

    void VulkanComputePipeline::Bind(CommandBuffer& commandBuffer)
    {
        vk::CommandBuffer cmd = commandBuffer.GetBufferHandleAs<vk::CommandBuffer>();
        cmd.bindPipeline(vk::PipelineBindPoint::eCompute, m_Pipeline);
    }

    VulkanComputePipeline::~VulkanComputePipeline()
    {
        DeletionQueue::Frame().PushFunction(
            [device = m_Device.GetDevice(), pipeline = m_Pipeline, layout = m_PipelineLayout]()
            {
                device.destroyPipeline(pipeline);
                device.destroyPipelineLayout(layout);
            });
    }
} // namespace BeeEngine::Internal
