//
// Created by Александр Лебедев on 27.06.2023.
//

#pragma once

#include "Renderer/GraphicsBuffer.h"
#include "Core/TypeDefines.h"
#include "Renderer/BufferLayout.h"
#include "vulkan/vulkan.h"

namespace BeeEngine::Internal
{
    class VulkanVertexBuffer: public GraphicsBuffer
    {
    public:
        VulkanVertexBuffer(uint32_t numberOfVertices, in<BufferLayout> layout);

        [[nodiscard]] uint32_t GetSize() const override
        {
            return m_Size;
        }

        [[nodiscard]] const BufferLayout& GetLayout() const override
        {
            return m_Layout;
        }

        void SetLayout(in<BufferLayout> layout) override
        {
            m_Layout = layout;
            InitLayout();
        }

        void SetData(gsl::span<byte> data) override;

        [[nodiscard]] const std::vector<VkVertexInputBindingDescription>& GetBindingDescriptions() const
        {
            return m_BindingDescriptions;
        }

        [[nodiscard]] const std::vector<VkVertexInputAttributeDescription>& GetAttributeDescriptions() const
        {
            return m_AttributeDescriptions;
        }
    private:
        void InitLayout();
        VkFormat ConvertShaderTypeToVulkan(ShaderDataType type);

        uint32_t m_Size;
        BufferLayout m_Layout;
        std::vector<VkVertexInputBindingDescription> m_BindingDescriptions;
        std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions;
    };
}


