#include "VulkanModel.h"
#include "vk_mem_alloc.h"

namespace BeeEngine::Internal
{
    VulkanModel::VulkanModel(VulkanGraphicsDevice& graphicsDevice, const std::vector<Vertex>& vertices)
    : m_GraphicsDevice(graphicsDevice)
    {
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

    void VulkanModel::Unbind()
    {

    }

    void VulkanModel::CreateVertexBuffers(in<std::vector<Vertex>> vertices)
    {
        m_VertexCount = vertices.size();
        BeeExpects(m_VertexCount >= 3);

        VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;
        m_GraphicsDevice.CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                      VMA_MEMORY_USAGE_AUTO,
                                      m_VertexBuffer);
        m_GraphicsDevice.CopyToBuffer({(byte*)vertices.data(), bufferSize}, m_VertexBuffer);
    }

    std::vector<VkVertexInputBindingDescription> VulkanModel::Vertex::GetBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> VulkanModel::Vertex::GetAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, Position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, Normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, Color);
        return attributeDescriptions;
    }
}