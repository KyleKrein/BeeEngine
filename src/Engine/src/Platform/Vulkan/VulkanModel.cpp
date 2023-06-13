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
        vkDestroyBuffer(m_GraphicsDevice.GetDevice(), m_VertexBuffer, nullptr);
        vkFreeMemory(m_GraphicsDevice.GetDevice(), m_VertexBufferMemory, nullptr);
    }
}