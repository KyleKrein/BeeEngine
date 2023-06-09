//
// Created by alexl on 09.06.2023.
//

#include "VulkanPipeline.h"
#include "Windowing/WindowHandler/WindowHandler.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanRenderPass.h"

namespace BeeEngine::Internal
{

    VulkanPipeline::VulkanPipeline(const GraphicsPipelineInBundle &specification)
    : m_Device((*(VulkanGraphicsDevice*)&(WindowHandler::GetInstance()->GetGraphicsDevice())).GetDevice())
    {
        vk::GraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.flags = vk::PipelineCreateFlags();

        std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

        //Vertex input
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.flags = vk::PipelineVertexInputStateCreateFlags();
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        pipelineInfo.pVertexInputState = &vertexInputInfo;

        //Input assembly
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.flags = vk::PipelineInputAssemblyStateCreateFlags();
        inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
        pipelineInfo.pInputAssemblyState = &inputAssembly;

        vk::PipelineShaderStageCreateInfo vertexShaderStageInfo{};
        vertexShaderStageInfo.flags = vk::PipelineShaderStageCreateFlags();
        vertexShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
        vertexShaderStageInfo.module = specification.VertexShader->GetHandle();
        vertexShaderStageInfo.pName = "main";
        shaderStages.push_back(vertexShaderStageInfo);

        //Viewport and scissors
        vk::Viewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)specification.ViewportWidth;
        viewport.height = (float)specification.ViewportHeight;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vk::Rect2D scissor{};
        scissor.offset = vk::Offset2D{0, 0};
        scissor.extent = vk::Extent2D{specification.ViewportWidth, specification.ViewportHeight};

        vk::PipelineViewportStateCreateInfo viewportState{};
        viewportState.flags = vk::PipelineViewportStateCreateFlags();
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;
        pipelineInfo.pViewportState = &viewportState;

        //Rasterizer
        vk::PipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.flags = vk::PipelineRasterizationStateCreateFlags();
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = vk::PolygonMode::eFill;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = vk::CullModeFlagBits::eBack;
        rasterizer.frontFace = vk::FrontFace::eClockwise;
        rasterizer.depthBiasEnable = VK_FALSE;
        pipelineInfo.pRasterizationState = &rasterizer;

        //Fragment input
        vk::PipelineShaderStageCreateInfo fragmentShaderStageInfo{};
        vertexShaderStageInfo.flags = vk::PipelineShaderStageCreateFlags();
        vertexShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
        vertexShaderStageInfo.module = specification.FragmentShader->GetHandle();
        vertexShaderStageInfo.pName = "main";
        shaderStages.push_back(fragmentShaderStageInfo);

        pipelineInfo.stageCount = shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();

        //Multisampling
        vk::PipelineMultisampleStateCreateInfo multisampling{};
        multisampling.flags = vk::PipelineMultisampleStateCreateFlags();
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
        pipelineInfo.pMultisampleState = &multisampling;

        //Color blending
        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                              vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.blendEnable = VK_FALSE;
        vk::PipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.flags = vk::PipelineColorBlendStateCreateFlags();
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = vk::LogicOp::eCopy;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;
        pipelineInfo.pColorBlendState = &colorBlending;

        CreateLayout();

        pipelineInfo.layout = m_PipelineLayout;

        //RenderPass
        m_RenderPass = VulkanRenderPass(specification.SwapChainImageFormat);
        pipelineInfo.renderPass = m_RenderPass.GetHandle();

        //Extra stuff
        pipelineInfo.basePipelineHandle = nullptr;

        //Create pipeline
        try
        {
            m_Pipeline = m_Device.createGraphicsPipeline(nullptr, pipelineInfo).value;
        }
        catch (vk::SystemError &err)
        {
            BeeCoreError("Failed to create graphics pipeline: {0}", err.what());
        }

    }

    void VulkanPipeline::CreateLayout()
    {
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.flags = vk::PipelineLayoutCreateFlags();
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        try
        {
            m_PipelineLayout = m_Device.createPipelineLayout(pipelineLayoutInfo);
        }
        catch (vk::SystemError &err)
        {
            BeeCoreError("Failed to create pipeline layout: {0}", err.what());
        }
    }

    VulkanPipeline::~VulkanPipeline()
    {
        m_Device.destroyPipeline(m_Pipeline);
        m_Device.destroyPipelineLayout(m_PipelineLayout);
    }
}
