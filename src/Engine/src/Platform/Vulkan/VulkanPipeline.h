//
// Created by alexl on 09.06.2023.
//

#pragma once
#if defined(BEE_COMPILE_VULKAN)
#include "Core/TypeDefines.h"
#include "Renderer/Shader.h"
#include "vulkan/vulkan.hpp"
#include "Utils/ShaderConverter.h"

namespace BeeEngine::Internal
{
    /*struct GraphicsPipelineInBundle
    {
        uint32_t ViewportWidth;
        uint32_t ViewportHeight;
        vk::Format SwapChainImageFormat;
        Ref<VulkanShaderModule> VertexShader;
        Ref<VulkanShaderModule> FragmentShader;
        vk::Device Device;
    };*/
    struct PipelineConfigInfo {
        PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        vk::PipelineViewportStateCreateInfo viewportInfo {};
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo {};
        vk::PipelineRasterizationStateCreateInfo rasterizationInfo {};
        vk::PipelineMultisampleStateCreateInfo multisampleInfo {};
        vk::PipelineColorBlendAttachmentState colorBlendAttachment {};
        vk::PipelineColorBlendStateCreateInfo colorBlendInfo {};
        vk::PipelineDepthStencilStateCreateInfo depthStencilInfo {};
        std::vector<vk::DynamicState> dynamicStateEnables {};
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo {};
        vk::PipelineLayout pipelineLayout = nullptr;
        vk::RenderPass renderPass = nullptr;
        uint32_t subpass = 0;

        std::vector<VkVertexInputBindingDescription> bindingDescriptions {};
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions {};
    };
    class VulkanGraphicsDevice;
    class VulkanPipeline
    {
    public:
        //VulkanPipeline(vk::Device& device, const GraphicsPipelineInBundle& specification);

        VulkanPipeline(
                in<vk::Device> device,
                in<std::string> vertFilepath,
                in<std::string> fragFilepath,
                in<PipelineConfigInfo> configInfo);
        ~VulkanPipeline();
        vk::Pipeline& GetHandle()
        {
            return m_Pipeline;
        }
        /*
        vk::PipelineLayout& GetLayout()
        {
            return m_PipelineLayout;
        }
        VulkanRenderPass& GetRenderPass()
        {
            return m_RenderPass;
        }
        */
        static void DefaultPipelineConfigInfo(out<PipelineConfigInfo> configInfo);


        void Bind(VkCommandBuffer commandBuffer);


    private:
        static std::vector<char> readFile(const std::string& filepath);

    private:
        vk::Pipeline m_Pipeline;
        //VulkanGraphicsDevice* m_GraphicsDevice;
        in<vk::Device> m_Device;
        //vk::PipelineLayout m_PipelineLayout;
        //VulkanRenderPass m_RenderPass;

        vk::ShaderModule m_VertexShaderModule;
        vk::ShaderModule m_FragmentShaderModule;
        void CreateLayout();
        void CreateGraphicsPipeline(
                const std::string& vertFilepath,
                const std::string& fragFilepath,
                const PipelineConfigInfo& configInfo);

        void CreateShaderModule(const std::vector<uint32_t>& code, vk::ShaderModule& shaderModule);
        std::vector<uint32_t> CompileShaderToSPRIV(const std::vector<char>& file, std::string_view newFilepath, ShaderStage shaderType);
    };
}
#endif