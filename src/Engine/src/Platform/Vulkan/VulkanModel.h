#pragma once

#include "VulkanGraphicsDevice.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
namespace BeeEngine::Internal
{
    class VulkanModel
    {
    public:
        struct Vertex
        {
            glm::vec3 Position;
            glm::vec3 Normal;
            glm::vec3 Color;

            static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
        };
        VulkanModel(VulkanGraphicsDevice& graphicsDevice, const std::vector<Vertex>& vertices);
        ~VulkanModel();
        VulkanModel(const VulkanModel& other) = delete;
        VulkanModel& operator=(const VulkanModel& other ) = delete;

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);
        void Unbind();

        [[nodiscard]] uint32_t GetVertexCount() const
        {
            return m_VertexCount;
        }

    private:
        void CreateVertexBuffers(in<std::vector<Vertex>> vertices);

        VulkanGraphicsDevice& m_GraphicsDevice;
        VulkanBuffer m_VertexBuffer;
        uint32_t m_VertexCount;
    };
}