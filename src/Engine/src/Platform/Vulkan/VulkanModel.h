#pragma once

#include "VulkanGraphicsDevice.h"
#include "Renderer/Mesh.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
namespace BeeEngine::Internal
{
    class VulkanModel: public Mesh
    {
    public:
        VulkanModel(VulkanGraphicsDevice& graphicsDevice, gsl::span<byte> vertices, uint32_t numberOfVertices, BufferLayout layout);
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

        [[nodiscard]] uint32_t GetVertexSize() const
        {
            return m_VertexSize;
        }

        [[nodiscard]] const BufferLayout& GetLayout() const
        {
            return m_Layout;
        }

        [[nodiscard]] const std::vector<VkVertexInputBindingDescription>& GetBindingDescriptions() const
        {
            return m_BindingDescriptions;
        }

        [[nodiscard]] const std::vector<VkVertexInputAttributeDescription>& GetAttributeDescriptions() const
        {
            return m_AttributeDescriptions;
        }

    private:
        void CreateVertexBuffers(gsl::span<byte> vertices);
        VkFormat ConvertShaderTypeToVulkan(ShaderDataType type);
        void InitLayout();

        VulkanGraphicsDevice& m_GraphicsDevice;
        VulkanBuffer m_VertexBuffer;
        uint32_t m_VertexCount;
        uint32_t m_VertexSize;
        BufferLayout m_Layout;
        std::vector<VkVertexInputBindingDescription> m_BindingDescriptions;
        std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions;
    };
}