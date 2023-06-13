#include "VulkanModel.h"

namespace BeeEngine::Internal
{
    VulkanModel::VulkanModel(VulkanGraphicsDevice& graphicsDevice, const std::vector<Vertex>& vertices)
    : m_GraphicsDevice(graphicsDevice)
    {
        CreateVertexBuffers(vertices);
    }
    VulkanModel::~VulkanModel()
    {
        vkDeviceWaitIdle(m_GraphicsDevice.GetDevice());
        vkDestroyBuffer(m_GraphicsDevice.GetDevice(), m_VertexBuffer, nullptr);
        vkFreeMemory(m_GraphicsDevice.GetDevice(), m_VertexBufferMemory, nullptr);
    }

    void VulkanModel::Draw(VkCommandBuffer commandBuffer)
    {
        vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
    }

    void VulkanModel::Bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers [] = {m_VertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    void VulkanModel::Unbind()
    {

    }

    void VulkanModel::CreateVertexBuffers(const std::vector<Vertex> &vertices)
    {
        m_VertexCount = vertices.size();
        BeeExpects(m_VertexCount >= 3);

        VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;
        m_GraphicsDevice.CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                      m_VertexBuffer, m_VertexBufferMemory);
        void* data;
        vkMapMemory(m_GraphicsDevice.GetDevice(), m_VertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
        vkUnmapMemory(m_GraphicsDevice.GetDevice(), m_VertexBufferMemory);
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
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = 0;
        return attributeDescriptions;
    }
}