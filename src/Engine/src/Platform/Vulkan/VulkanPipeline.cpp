//
// Created by alexl on 09.06.2023.
//

#include "Core/Logging/Log.h"
#include "VulkanPipeline.h"
#include "Utils/File.h"
#include <fstream>
#include "VulkanModel.h"

namespace BeeEngine::Internal
{

    /*VulkanPipeline::VulkanPipeline(vk::Device& device,const GraphicsPipelineInBundle &specification)
    : m_Device(device)
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
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
        inputAssemblyInfo.flags = vk::PipelineInputAssemblyStateCreateFlags();
        inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
        pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;

        vk::PipelineShaderStageCreateInfo vertexShaderInfo = {};
        vertexShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
        vertexShaderInfo.stage = vk::ShaderStageFlagBits::eVertex;
        vertexShaderInfo.module = specification.VertexShader->GetHandle();
        vertexShaderInfo.pName = "main";
        shaderStages.push_back(vertexShaderInfo);

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
        vk::PipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.flags = vk::PipelineRasterizationStateCreateFlags();
        rasterizer.depthClampEnable = VK_FALSE; //discard out of bounds fragments, don't clamp them
        rasterizer.rasterizerDiscardEnable = VK_FALSE; //This flag would disable fragment output
        rasterizer.polygonMode = vk::PolygonMode::eFill;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = vk::CullModeFlagBits::eBack;
        rasterizer.frontFace = vk::FrontFace::eClockwise;
        rasterizer.depthBiasEnable = VK_FALSE; //Depth bias can be useful in shadow maps.
        pipelineInfo.pRasterizationState = &rasterizer;

        //Fragment input
        vk::PipelineShaderStageCreateInfo fragmentShaderStageInfo{};
        fragmentShaderStageInfo.flags = vk::PipelineShaderStageCreateFlags();
        fragmentShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
        fragmentShaderStageInfo.module = specification.FragmentShader->GetHandle();
        fragmentShaderStageInfo.pName = "main";
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
        m_RenderPass = std::move(VulkanRenderPass(m_Device,specification.SwapChainImageFormat));
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

    }*/

    void VulkanPipeline::CreateLayout()
    {
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.flags = vk::PipelineLayoutCreateFlags();
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        try
        {
            //m_PipelineLayout = m_Device.createPipelineLayout(pipelineLayoutInfo);
        }
        catch (vk::SystemError &err)
        {
            BeeCoreError("Failed to create pipeline layout: {0}", err.what());
        }
    }

    VulkanPipeline::~VulkanPipeline()
    {
        //m_Device.destroyPipeline(m_Pipeline);
        //m_Device.destroyPipelineLayout(m_PipelineLayout);
        vkDeviceWaitIdle(m_Device);
        m_Device.destroyShaderModule(m_VertexShaderModule);
        m_Device.destroyShaderModule(m_FragmentShaderModule);
        m_Device.destroyPipeline(m_Pipeline);
    }

    VulkanPipeline::VulkanPipeline(in<vk::Device> device, in<std::string> vertFilepath,
                                   in<std::string> fragFilepath, in<PipelineConfigInfo> configInfo)
    : m_Device(device)
    {
        CreateGraphicsPipeline(vertFilepath, fragFilepath, configInfo);
    }

    std::vector<char> VulkanPipeline::readFile(const std::string &filepath)
    {
        std::ifstream file(filepath);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file: " + filepath);
        }

        size_t fileSize = File::Size(filepath);
        std::vector<char> buffer(fileSize);

        file.read(buffer.data(), fileSize);
        std::erase(buffer, '\0');

        file.close();
        return buffer;
    }

    void VulkanPipeline::CreateGraphicsPipeline(const std::string &vertFilepath, const std::string &fragFilepath,
                                                const PipelineConfigInfo &configInfo)
    {
        auto vertexCode = readFile(vertFilepath);
        auto fragmentCode = readFile(fragFilepath);

        auto vertCompiled = CompileShaderToSPRIV(vertexCode, vertFilepath + ".spv", ShaderStage::Vertex);
        auto fragCompiled = CompileShaderToSPRIV(fragmentCode, fragFilepath + ".spv", ShaderStage::Fragment);

        CreateShaderModule(vertCompiled, m_VertexShaderModule);
        CreateShaderModule(fragCompiled, m_FragmentShaderModule);

        vk::PipelineShaderStageCreateInfo shaderStages[2];
        shaderStages[0].sType = vk::StructureType::ePipelineShaderStageCreateInfo;
        shaderStages[0].stage = vk::ShaderStageFlagBits::eVertex;
        shaderStages[0].module = m_VertexShaderModule;
        shaderStages[0].pName = "main";
        shaderStages[0].flags = vk::PipelineShaderStageCreateFlags();
        shaderStages[0].pNext = nullptr;
        shaderStages[0].pSpecializationInfo = nullptr;
        shaderStages[1].sType = vk::StructureType::ePipelineShaderStageCreateInfo;
        shaderStages[1].stage = vk::ShaderStageFlagBits::eFragment;
        shaderStages[1].module = m_FragmentShaderModule;
        shaderStages[1].pName = "main";
        shaderStages[1].flags = vk::PipelineShaderStageCreateFlags();
        shaderStages[1].pNext = nullptr;
        shaderStages[1].pSpecializationInfo = nullptr;

        auto bindingDescription = VulkanModel::Vertex::GetBindingDescriptions();
        auto attributeDescriptions = VulkanModel::Vertex::GetAttributeDescriptions();
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescription.size());
        vertexInputInfo.pVertexAttributeDescriptions = reinterpret_cast<const vk::VertexInputAttributeDescription *>(attributeDescriptions.data());
        vertexInputInfo.pVertexBindingDescriptions = reinterpret_cast<const vk::VertexInputBindingDescription *>(bindingDescription.data());

        vk::GraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
        pipelineInfo.pViewportState = &configInfo.viewportInfo;
        pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
        pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
        pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
        pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
        pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;

        pipelineInfo.layout = configInfo.pipelineLayout;
        pipelineInfo.renderPass = configInfo.renderPass;
        pipelineInfo.subpass = configInfo.subpass;

        pipelineInfo.basePipelineIndex = -1;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        m_Device.createGraphicsPipelines(VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline);
    }

    void VulkanPipeline::CreateShaderModule(const std::vector<uint32_t> &code, vk::ShaderModule &shaderModule)
    {
        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
        createInfo.codeSize = code.size() * sizeof(uint32_t);
        createInfo.pCode = code.data();

        m_Device.createShaderModule(&createInfo, nullptr, &shaderModule);
    }
    void VulkanPipeline::DefaultPipelineConfigInfo(PipelineConfigInfo& inOutConfigInfo)
    {
        inOutConfigInfo.inputAssemblyInfo.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
        inOutConfigInfo.inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
        inOutConfigInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        inOutConfigInfo.viewportInfo.sType = vk::StructureType::ePipelineViewportStateCreateInfo;
        inOutConfigInfo.viewportInfo.viewportCount = 1;
        inOutConfigInfo.viewportInfo.pViewports= nullptr;
        inOutConfigInfo.viewportInfo.scissorCount= 1;
        inOutConfigInfo.viewportInfo.pScissors = nullptr;

        inOutConfigInfo.rasterizationInfo.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
        inOutConfigInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
        inOutConfigInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        inOutConfigInfo.rasterizationInfo.polygonMode = vk::PolygonMode::eFill;
        inOutConfigInfo.rasterizationInfo.lineWidth = 1.0f;
        inOutConfigInfo.rasterizationInfo.cullMode = vk::CullModeFlagBits::eNone;
        inOutConfigInfo.rasterizationInfo.frontFace = vk::FrontFace::eClockwise;
        inOutConfigInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
        inOutConfigInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
        inOutConfigInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
        inOutConfigInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

        inOutConfigInfo.multisampleInfo.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
        inOutConfigInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        inOutConfigInfo.multisampleInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
        inOutConfigInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
        inOutConfigInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
        inOutConfigInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
        inOutConfigInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

        inOutConfigInfo.colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                         vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        inOutConfigInfo.colorBlendAttachment.blendEnable = VK_FALSE;
        inOutConfigInfo.colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;   // Optional
        inOutConfigInfo.colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;  // Optional
        inOutConfigInfo.colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;              // Optional
        inOutConfigInfo.colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;   // Optional
        inOutConfigInfo.colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;  // Optional
        inOutConfigInfo.colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;              // Optional

        inOutConfigInfo.colorBlendInfo.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
        inOutConfigInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
        inOutConfigInfo.colorBlendInfo.logicOp = vk::LogicOp::eCopy;  // Optional
        inOutConfigInfo.colorBlendInfo.attachmentCount = 1;
        inOutConfigInfo.colorBlendInfo.pAttachments = &inOutConfigInfo.colorBlendAttachment;
        inOutConfigInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
        inOutConfigInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
        inOutConfigInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
        inOutConfigInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

        inOutConfigInfo.depthStencilInfo.sType = vk::StructureType::ePipelineDepthStencilStateCreateInfo;
        inOutConfigInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
        inOutConfigInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
        inOutConfigInfo.depthStencilInfo.depthCompareOp = vk::CompareOp::eLess;
        inOutConfigInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        inOutConfigInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
        inOutConfigInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
        inOutConfigInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
        inOutConfigInfo.depthStencilInfo.front = vk::StencilOpState{};  // Optional
        inOutConfigInfo.depthStencilInfo.back = vk::StencilOpState{};   // Optional

        inOutConfigInfo.dynamicStateEnables = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
        inOutConfigInfo.dynamicStateInfo.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
        inOutConfigInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(inOutConfigInfo.dynamicStateEnables.size());
        inOutConfigInfo.dynamicStateInfo.pDynamicStates = inOutConfigInfo.dynamicStateEnables.data();
        inOutConfigInfo.dynamicStateInfo.flags = vk::PipelineDynamicStateCreateFlags();
    }
    std::vector<uint32_t> VulkanPipeline::CompileShaderToSPRIV(const std::vector<char>& file, std::string_view newFilepath, ShaderStage shaderType)
    {
        if(File::Exists(newFilepath))
        {
            auto result = File::ReadBinaryFile(newFilepath);
            gsl::span<uint32_t> span((uint32_t *)result.data(), result.size() / sizeof(uint32_t));
            BeeCoreTrace("Loaded SPIRV from cache");
            return {span.begin(), span.end()};
        }
        std::vector<uint32_t> result;
        if (!ShaderConverter::GLSLtoSPV(shaderType, file.data(), result))
        {
            return result;
        }
        BeeCoreTrace("Compiled shader to SPIRV");
        File::WriteBinaryFile(newFilepath, {(std::byte*)result.data(), result.size() * sizeof(uint32_t)});
        return result;
    }

    void VulkanPipeline::Bind(VkCommandBuffer commandBuffer)
    {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
    }
}
