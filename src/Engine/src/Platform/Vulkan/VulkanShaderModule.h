//
// Created by Aleksandr on 02.03.2024.
//

#pragma once
#include "VulkanGraphicsDevice.h"
#include "Renderer/ShaderModule.h"


namespace BeeEngine::Internal
{

    class VulkanShaderModule: public ShaderModule
    {
    public:
        VulkanShaderModule(std::vector<uint32_t>& spirv, ShaderType type, BufferLayout&& layout);
        ~VulkanShaderModule() override;

        [[nodiscard]] ShaderType GetType() const override;

        [[nodiscard]] Scope<InstancedBuffer> CreateInstancedBuffer() override;

        [[nodiscard]] vk::ShaderModule GetShaderModule() const
        {
            return m_ShaderModule;
        }
        [[nodiscard]] vk::PipelineVertexInputStateCreateInfo GetVertexInputState() const
        {
            return m_VertexInputState;
        }
        [[nodiscard]] const std::vector<vk::DescriptorSetLayout>& GetDescriptorSetLayouts() const
        {
            return m_DescriptorSetLayouts;
        }
        struct ColorAttachmentData
        {
            const std::vector<vk::Format>& formats;
            const std::vector<vk::PipelineColorBlendAttachmentState>& attachments;
        };
        [[nodiscard]] ColorAttachmentData GetColorAttachmentData() const
        {
            BeeExpects(m_Type == ShaderType::Fragment);
            return {m_ColorAttachmentFormats, m_ColorBlendAttachments};
        }

    private:
        void InitData();

        ShaderType m_Type;
        BufferLayout m_Layout;
        VulkanGraphicsDevice& m_GraphicsDevice;
        vk::ShaderModule m_ShaderModule;
        size_t m_OneInstanceSize;
        vk::VertexInputBindingDescription m_BindingDescriptions[2];
        vk::VertexInputBindingDescription& m_VertexBindingDescription = m_BindingDescriptions[0];
        std::vector<vk::VertexInputAttributeDescription> m_VertexAttributeDescriptions;
        vk::VertexInputBindingDescription& m_InstanceBindingDescription = m_BindingDescriptions[1];
        //std::vector<vk::VertexInputAttributeDescription> m_InstanceAttributeDescriptions;
        vk::PipelineVertexInputStateCreateInfo m_VertexInputState;
        std::vector<vk::DescriptorSetLayout> m_DescriptorSetLayouts;
        std::vector<vk::Format> m_ColorAttachmentFormats;
        std::vector<vk::PipelineColorBlendAttachmentState> m_ColorBlendAttachments;
    };

} // Internal
// BeeEngine
