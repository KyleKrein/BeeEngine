#include "VulkanModel.h"
#include "vk_mem_alloc.h"

namespace BeeEngine::Internal
{
    VulkanModel::VulkanModel(VulkanGraphicsDevice& graphicsDevice, gsl::span<byte> vertices, uint32_t numberOfVertices, BufferLayout layout)
    : m_GraphicsDevice(graphicsDevice), m_VertexCount(numberOfVertices), m_Layout(std::move(layout))
    {
        InitLayout();
        CreateVertexBuffers(vertices);
    }
    VulkanModel::~VulkanModel()
    {
        //vkDeviceWaitIdle(m_GraphicsDevice.GetDevice());
        //vkDestroyBuffer(m_GraphicsDevice.GetDevice(), m_VertexBuffer, nullptr);
        //vkFreeMemory(m_GraphicsDevice.GetDevice(), m_VertexBufferMemory, nullptr);
    }

    void VulkanModel::Draw(VkCommandBuffer commandBuffer)
    {
        vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
    }

    void VulkanModel::Bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers [] = {m_VertexBuffer.Buffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    void VulkanModel::InitLayout()
    {
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
            m_VertexSize += m_Layout.GetElements()[i].GetSize();
        }
    }

    VkFormat VulkanModel::ConvertShaderTypeToVulkan(ShaderDataType type)
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
        return VK_FORMAT_UNDEFINED;
    }

    void VulkanModel::Unbind()
    {

    }

    void VulkanModel::CreateVertexBuffers(gsl::span<byte> vertices)
    {
        BeeExpects(m_VertexCount >= 3);

        VkDeviceSize bufferSize = m_VertexSize * m_VertexCount;
        m_GraphicsDevice.CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                      VMA_MEMORY_USAGE_AUTO,
                                      m_VertexBuffer);
        m_GraphicsDevice.CopyToBuffer({(byte*)vertices.data(), bufferSize}, m_VertexBuffer);
    }
}