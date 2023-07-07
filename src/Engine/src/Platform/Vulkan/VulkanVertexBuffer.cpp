//
// Created by Александр Лебедев on 27.06.2023.
//

#include "VulkanVertexBuffer.h"

namespace BeeEngine::Internal
{

    VulkanVertexBuffer::VulkanVertexBuffer(uint32_t numberOfVertices, in<BufferLayout> layout)
    {
        m_Layout = layout;
        m_Size = numberOfVertices * layout.GetStride();
        InitLayout();
    }

    void VulkanVertexBuffer::InitLayout()
    {
        m_BindingDescriptions.clear();
        m_BindingDescriptions.resize(1);
        m_BindingDescriptions[0].binding = 0;
        m_BindingDescriptions[0].stride = m_Layout.GetStride();
        m_BindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        m_AttributeDescriptions.clear();
        m_AttributeDescriptions.resize(m_Layout.GetElements().size());
        for (size_t i = 0; i < m_Layout.GetElements().size(); i++)
        {
            m_AttributeDescriptions[i].binding = 0;
            m_AttributeDescriptions[i].location = i;
            m_AttributeDescriptions[i].format = ConvertShaderTypeToVulkan(m_Layout.GetElements()[i].GetType());
            m_AttributeDescriptions[i].offset = m_Layout.GetElements()[i].GetOffset();
        }
    }

    VkFormat VulkanVertexBuffer::ConvertShaderTypeToVulkan(ShaderDataType type)
    {
        for (const auto& element : m_Layout)
        {
            switch (element.GetType())
            {
                case ShaderDataType::Float:
                    return VK_FORMAT_R32_SFLOAT;
                case ShaderDataType::Float2:
                    return VK_FORMAT_R32G32_SFLOAT;
                case ShaderDataType::Float3:
                    return VK_FORMAT_R32G32B32_SFLOAT;
                case ShaderDataType::Float4:
                    return VK_FORMAT_R32G32B32A32_SFLOAT;
                case ShaderDataType::Int:
                    return VK_FORMAT_R32_SINT;
                case ShaderDataType::Int2:
                    return VK_FORMAT_R32G32_SINT;
                case ShaderDataType::Int3:
                    return VK_FORMAT_R32G32B32_SINT;
                case ShaderDataType::Int4:
                    return VK_FORMAT_R32G32B32A32_SINT;
                case ShaderDataType::Bool:
                    return VK_FORMAT_R8_SINT;
                case ShaderDataType::Mat3:
                    //todo:implement mat3 and mat4
                case ShaderDataType::Mat4:
                default:
                    BeeCoreError("Unknown ShaderDataType!");
            }
        }
    }
}