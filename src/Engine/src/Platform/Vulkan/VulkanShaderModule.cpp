//
// Created by Aleksandr on 02.03.2024.
//

#include "VulkanShaderModule.h"

#include "VulkanInstancedBuffer.h"
#include <map>


namespace BeeEngine::Internal
{
    VulkanShaderModule::VulkanShaderModule(std::vector<uint32_t>& spirv, ShaderType type, BufferLayout&& layout)
        : m_Type(type), m_Layout(std::move(layout)), m_GraphicsDevice(VulkanGraphicsDevice::GetInstance()), m_OneInstanceSize(m_Layout.GetStride())
    {
        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.setCodeSize(spirv.size() * sizeof(uint32_t));
        createInfo.setPCode(spirv.data());

        try
        {
            m_ShaderModule = m_GraphicsDevice.GetDevice().createShaderModule(createInfo);
            BeeCoreTrace("Shader module of type {0} created", type);
        }
        catch(const vk::SystemError& e)
        {
            BeeCoreError("Failed to create shader module: {}", e.what());
        }
        InitData();
    }

    VulkanShaderModule::~VulkanShaderModule()
    {
        DeletionQueue::Frame().PushFunction([device = m_GraphicsDevice.GetDevice(), shaderModule = m_ShaderModule, descriptorSetLayouts = std::move(m_DescriptorSetLayouts)](){
            if(shaderModule)
                device.destroyShaderModule(shaderModule);
            for(auto& layout : descriptorSetLayouts)
            {
                device.destroyDescriptorSetLayout(layout);
            }
        });
    }

    ShaderType VulkanShaderModule::GetType() const
    {
        return m_Type;
    }

    Scope<InstancedBuffer> VulkanShaderModule::CreateInstancedBuffer()
    {
        static constexpr size_t MAX_INSTANCED_BUFFER_COUNT = 10000;
        if (m_Type != ShaderType::Vertex)
        {
            BeeCoreError("Instanced buffer can be created only for vertex shader");
            return nullptr;
        }
        BeeCoreTrace("Creating instanced buffer");
        return CreateScope<VulkanInstancedBuffer>(MAX_INSTANCED_BUFFER_COUNT * m_OneInstanceSize);
    }

    constexpr static vk::Format ShaderDataTypeToVulkan(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:
                return vk::Format::eR32Sfloat;
            case ShaderDataType::Float2:
                return vk::Format::eR32G32Sfloat;
            case ShaderDataType::Float3:
                return vk::Format::eR32G32B32Sfloat;
            case ShaderDataType::Float4:
                return vk::Format::eR32G32B32A32Sfloat;
            case ShaderDataType::UInt:
                return vk::Format::eR32Uint;
            case ShaderDataType::UInt2:
                return vk::Format::eR32G32Uint;
            case ShaderDataType::UInt3:
                return vk::Format::eR32G32B32Uint;
            case ShaderDataType::UInt4:
                return vk::Format::eR32G32B32A32Uint;
            case ShaderDataType::Int:
                return vk::Format::eR32Sint;
            case ShaderDataType::Int2:
                return vk::Format::eR32G32Sint;
            case ShaderDataType::Int3:
                return vk::Format::eR32G32B32Sint;
            case ShaderDataType::Int4:
                return vk::Format::eR32G32B32A32Sint;
            case ShaderDataType::Bool:
            case ShaderDataType::Half:
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4:
            case ShaderDataType::NoneData:
                BeeCoreError("Unknown ShaderDataType");
            break;
        }
        return vk::Format::eUndefined;
    }

    constexpr static vk::DescriptorType ShaderUniformDataTypeToVulkan(ShaderUniformDataType type)
    {
        switch (type)
        {
            case ShaderUniformDataType::Sampler:
                return vk::DescriptorType::eSampler;
            case ShaderUniformDataType::Data:
                return vk::DescriptorType::eUniformBuffer;
            case ShaderUniformDataType::SampledTexture:
                return vk::DescriptorType::eSampledImage;
            case ShaderUniformDataType::Unknown:
            default:
            BeeCoreError("Unknown ShaderUniformDataType");
            break;
        }
        return vk::DescriptorType::eUniformBuffer;
    }

    vk::ShaderStageFlags ShaderTypeToShaderStageFlagBits(ShaderType type)
    {
        switch (type)
        {
            case ShaderType::Vertex:
                //return vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
            case ShaderType::Fragment:
                return vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
            case ShaderType::Compute:
                return vk::ShaderStageFlagBits::eCompute;
            default:
                break;
        }
        BeeCoreError("Unknown shader type");
        return vk::ShaderStageFlagBits::eVertex;
    }

    vk::Format ColorAttachmentFormatToVulkan(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:
                return vk::Format::eR32Sfloat;
            case ShaderDataType::Float4:
                return vk::Format::eR8G8B8A8Unorm;
            default:
                BeeCoreError("Unknown ColorAttachmentFormat");
                return vk::Format::eUndefined;
        }
    }

    void VulkanShaderModule::InitData()
    {
        if(m_Type == ShaderType::Vertex)
        {
            m_VertexBindingDescription.stride = m_Layout.GetStride();
            m_VertexBindingDescription.binding = 0;
            m_VertexBindingDescription.inputRate = vk::VertexInputRate::eVertex;
            auto& inElements = m_Layout.GetInputElements();
            for (auto& element: inElements)
            {
                vk::VertexInputAttributeDescription attribute{};
                attribute.format = ShaderDataTypeToVulkan(element.GetType());
                attribute.offset = element.GetOffset();
                attribute.location = element.GetLocation();
                attribute.binding = 0;
                m_VertexAttributeDescriptions.push_back(attribute);
            }

            //Instance buffer
            m_InstanceBindingDescription.stride = m_Layout.GetInstancedStride();
            if(m_InstanceBindingDescription.stride == 0)
            {
                m_VertexInputState = vk::PipelineVertexInputStateCreateInfo{};
                m_VertexInputState.vertexBindingDescriptionCount = 1;
                m_VertexInputState.pVertexBindingDescriptions = m_BindingDescriptions;
                m_VertexInputState.vertexAttributeDescriptionCount = m_VertexAttributeDescriptions.size();
                m_VertexInputState.pVertexAttributeDescriptions = m_VertexAttributeDescriptions.data();
                goto uniforms;
            }
            m_InstanceBindingDescription.binding = 1;
            m_InstanceBindingDescription.inputRate = vk::VertexInputRate::eInstance;
            auto& instancedElements = m_Layout.GetInstancedElements();
            for (auto& element: instancedElements)
            {
                vk::VertexInputAttributeDescription attribute{};
                attribute.format = ShaderDataTypeToVulkan(element.GetType());
                attribute.offset = element.GetOffset();
                attribute.location = element.GetLocation();
                attribute.binding = 1;
                m_VertexAttributeDescriptions.push_back(attribute);
            }
            m_VertexInputState = vk::PipelineVertexInputStateCreateInfo{};
            m_VertexInputState.vertexBindingDescriptionCount = 2;
            m_VertexInputState.pVertexBindingDescriptions = m_BindingDescriptions;
            m_VertexInputState.vertexAttributeDescriptionCount = m_VertexAttributeDescriptions.size();
            m_VertexInputState.pVertexAttributeDescriptions = m_VertexAttributeDescriptions.data();
        }
        else if(m_Type == ShaderType::Fragment)
        {
            auto& outputs = m_Layout.GetOutputElements();
            m_ColorAttachmentFormats.reserve(outputs.size());
            m_ColorBlendAttachments.reserve(outputs.size());
            for(auto& element : outputs)
            {
                m_ColorAttachmentFormats.push_back(ColorAttachmentFormatToVulkan(element.GetType()));
            }
            for(auto format : m_ColorAttachmentFormats)
            {
                vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
                colorBlendAttachment.blendEnable = vk::False;
                colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                    vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
                /*switch (format)
                {
                    case vk::Format::eR32Sfloat:
                    {
                        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR;
                    }break;
                    case vk::Format::eR8G8B8A8Unorm:
                    {
                        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
                    }break;
                    default:
                        BeeCoreError("Unknown color attachment format");
                }*/
                m_ColorBlendAttachments.push_back(colorBlendAttachment);
            }
        }
        uniforms:
        auto& uniformElements = m_Layout.GetUniformElements();
        if(uniformElements.empty())
            return;

        auto device = m_GraphicsDevice.GetDevice();

        std::map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> bindings;
        for(auto& element : uniformElements)
        {
            vk::DescriptorSetLayoutBinding binding{};
            binding.binding = element.GetLocation();
            binding.descriptorType = ShaderUniformDataTypeToVulkan(element.GetType());
            binding.descriptorCount = 1;
            binding.stageFlags = ShaderTypeToShaderStageFlagBits(m_Type);
            binding.pImmutableSamplers = nullptr;
            bindings[element.GetBindingSet()].push_back(binding);
        }
        for(auto& [index, binding] : bindings)
        {
            vk::DescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.bindingCount = static_cast<uint32_t>(binding.size());
            layoutInfo.pBindings = binding.data();
            m_DescriptorSetLayouts.push_back(device.createDescriptorSetLayout(layoutInfo));
        }
    }
}
