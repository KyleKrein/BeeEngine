//
// Created by Aleksandr on 10.03.2024.
//

#pragma once
#include "Renderer/BindingSet.h"
#include <vulkan/vulkan.hpp>

#include "VulkanGraphicsDevice.h"

namespace BeeEngine::Internal
{

    class VulkanBindingSet final : public BindingSet
    {
    public:
        VulkanBindingSet(std::initializer_list<BindingSetElement> elements);
        VulkanBindingSet(std::vector<BindingSetElement> elements);
        void Bind(CommandBuffer& cmd, uint32_t index, Pipeline& pipeline) const override;

        ~VulkanBindingSet() override;

    private:
        vk::DescriptorSet m_DescriptorSet;
        vk::DescriptorSetLayout m_DescriptorSetLayout;
        VulkanGraphicsDevice& m_GraphicsDevice;
    };

} // namespace BeeEngine::Internal
