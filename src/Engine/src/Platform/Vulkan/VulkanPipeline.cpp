//
// Created by Aleksandr on 09.03.2024.
//

#include "VulkanPipeline.h"

#include <utility>

#include "Renderer/CommandBuffer.h"

namespace BeeEngine::Internal
{
    VulkanPipeline* VulkanPipeline::s_CurrentPipeline = nullptr;
    VulkanPipeline::VulkanPipeline(const Ref<ShaderModule>& vertexShader, const Ref<ShaderModule>& fragmentShader)
        : m_Device(VulkanGraphicsDevice::GetInstance()), m_VertexShader(std::move(std::static_pointer_cast<VulkanShaderModule>(vertexShader))),
        m_FragmentShader(std::move(std::static_pointer_cast<VulkanShaderModule>(fragmentShader)))
    {
        vk::PipelineShaderStageCreateInfo shaderStages[2];

        shaderStages[0].stage = vk::ShaderStageFlagBits::eVertex;
        shaderStages[0].module = m_VertexShader->GetShaderModule();
        shaderStages[0].pName = "main";

        shaderStages[1].stage = vk::ShaderStageFlagBits::eFragment;
        shaderStages[1].module = m_FragmentShader->GetShaderModule();
        shaderStages[1].pName = "main";

        auto vertexInputInfo = m_VertexShader->GetVertexInputState();

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
        inputAssembly.primitiveRestartEnable = vk::False;

        std::array dynamicStates = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
        };

        vk::PipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.dynamicStateCount = dynamicStates.size();
        dynamicState.pDynamicStates = dynamicStates.data();

        vk::PipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.depthClampEnable = vk::False;
        rasterizer.rasterizerDiscardEnable = vk::False;
        rasterizer.polygonMode = vk::PolygonMode::eFill;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = vk::CullModeFlagBits::eNone;
        rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
        rasterizer.depthBiasEnable = vk::False;

        vk::PipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sampleShadingEnable = vk::False;
        multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

        auto descriptorSetLayouts = m_VertexShader->GetDescriptorSetLayouts();
        auto fragmentDescriptorSetLayouts = m_FragmentShader->GetDescriptorSetLayouts();
        descriptorSetLayouts.insert(descriptorSetLayouts.end(), fragmentDescriptorSetLayouts.begin(),
            fragmentDescriptorSetLayouts.end());
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        auto device = m_Device.GetDevice();

        m_PipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);

        vk::PipelineViewportStateCreateInfo viewportState{};

        vk::Viewport viewport{};
        vk::Rect2D scissor{};
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        vk::PipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.depthTestEnable = vk::True;
        depthStencil.depthWriteEnable = vk::True;
        depthStencil.depthCompareOp = vk::CompareOp::eLess;
        depthStencil.depthBoundsTestEnable = vk::False;
        depthStencil.stencilTestEnable = vk::False;

        auto [colorAttachmentFormats, colorBlendAttachments] = m_FragmentShader->GetColorAttachmentData();

        vk::PipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.logicOpEnable = vk::False;
        colorBlending.logicOp = vk::LogicOp::eCopy;
        colorBlending.attachmentCount = colorAttachmentFormats.size();
        colorBlending.pAttachments = colorBlendAttachments.data();
        colorBlending.setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});

        vk::PipelineRenderingCreateInfo renderingInfo{};
        renderingInfo.colorAttachmentCount = colorAttachmentFormats.size();
        renderingInfo.pColorAttachmentFormats = colorAttachmentFormats.data();
        renderingInfo.depthAttachmentFormat = vk::Format::eD32Sfloat;

        vk::GraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.pNext = &renderingInfo;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = m_PipelineLayout;
        pipelineInfo.renderPass = nullptr;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = nullptr;

        auto result = device.createGraphicsPipeline(nullptr, pipelineInfo);
        if (result.result != vk::Result::eSuccess)
        {
            BeeCoreError("Failed to create graphics pipeline!");
        }
        m_Pipeline = result.value;
    }

    void VulkanPipeline::Bind(CommandBuffer& commandBuffer)
    {
        vk::CommandBuffer cmd = commandBuffer.GetBufferHandleAs<vk::CommandBuffer>();
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline);
        s_CurrentPipeline = this;
    }

    VulkanPipeline::~VulkanPipeline()
    {
        DeletionQueue::Frame().PushFunction([device = m_Device.GetDevice(), pipeline = m_Pipeline, layout = m_PipelineLayout]()
        {
            device.destroyPipeline(pipeline);
            device.destroyPipelineLayout(layout);
        });
    }
}
