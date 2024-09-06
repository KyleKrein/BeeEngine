//
// Created by Aleksandr on 10.03.2024.
//

#include "VulkanBindingSet.h"

#include "Renderer/CommandBuffer.h"
#include "Renderer/IBindable.h"
#include "VulkanPipeline.h"

namespace BeeEngine::Internal
{
    vk::PipelineBindPoint GetPipelineBindPoint(PipelineType type)
    {
        switch (type)
        {
            case PipelineType::Graphics:
                return vk::PipelineBindPoint::eGraphics;
            case PipelineType::Compute:
                return vk::PipelineBindPoint::eCompute;
            // case PipelineType::RayTracing:
            // return vk::PipelineBindPoint::eRayTracingKHR;
            default:
                return vk::PipelineBindPoint::eGraphics;
        }
    }
    VulkanBindingSet::VulkanBindingSet(std::initializer_list<BindingSetElement> elements)
        : VulkanBindingSet(std::vector(elements))
    {
    }
    VulkanBindingSet::VulkanBindingSet(std::vector<BindingSetElement> elements)
        : BindingSet(BeeMove(elements)), m_GraphicsDevice(VulkanGraphicsDevice::GetInstance())
    {
        std::vector<vk::DescriptorSetLayoutBinding> bindings;
        size_t bindingIndex = 0;
        if (!m_Elements.empty() && m_Elements[0].Binding != 0)
        {
            bindingIndex = m_Elements[0].Binding;
        }
        for (const auto& element : m_Elements)
        {
            auto binding = element.Data.GetBindGroupLayoutEntry();
            for (auto& entry : binding)
            {
                auto& vkEntry = std::get<vk::DescriptorSetLayoutBinding>(entry);
                vkEntry.binding = bindingIndex++;
                bindings.push_back(vkEntry);
            }
        }
        vk::DescriptorSetLayoutCreateInfo layoutInfo({}, (uint32_t)bindings.size(), bindings.data());
        m_DescriptorSetLayout = m_GraphicsDevice.GetDevice().createDescriptorSetLayout(layoutInfo);
        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &m_DescriptorSetLayout;
        m_GraphicsDevice.CreateDescriptorSet(allocInfo, &m_DescriptorSet);
        // update
        if (!m_Elements.empty() && m_Elements[0].Binding != 0)
        {
            bindingIndex = m_Elements[0].Binding;
        }
        else
        {
            bindingIndex = 0;
        }

        std::vector<vk::WriteDescriptorSet> descriptorWrites;
        for (const auto& element : m_Elements)
        {
            auto binding = element.Data.GetBindGroupEntry();
            for (auto& entry : binding)
            {
                auto& writeDescriptorSet = std::get<vk::WriteDescriptorSet>(entry);
                writeDescriptorSet.dstSet = m_DescriptorSet;
                writeDescriptorSet.dstBinding = bindingIndex++;
                writeDescriptorSet.dstArrayElement = 0;
                // writeDescriptorSet.descriptorType = vkEntry.descriptorType;
                // writeDescriptorSet.descriptorCount = vkEntry.descriptorCount;
                // writeDescriptorSet.pBufferInfo = vkEntry.pBufferInfo;
                // writeDescriptorSet.pImageInfo = vkEntry.pImageInfo;
                // writeDescriptorSet.pTexelBufferView = vkEntry.pTexelBufferView;
                descriptorWrites.push_back(writeDescriptorSet);
            }
        }
        m_GraphicsDevice.GetDevice().updateDescriptorSets(descriptorWrites, nullptr);
    }

    void VulkanBindingSet::Bind(CommandBuffer& cmd, uint32_t index, Pipeline& pipeline) const
    {
        auto commandBuffer = cmd.GetBufferHandleAs<vk::CommandBuffer>();
        commandBuffer.bindDescriptorSets(GetPipelineBindPoint(pipeline.GetType()),
                                         ((VulkanPipeline&)pipeline).GetPipelineLayout(),
                                         index,
                                         1,
                                         &m_DescriptorSet,
                                         0,
                                         nullptr);
    }

    VulkanBindingSet::~VulkanBindingSet()
    {
        DeletionQueue::Frame().PushFunction(
            [descriptorSet = m_DescriptorSet, descriptorLayout = m_DescriptorSetLayout]()
            {
                auto& device = VulkanGraphicsDevice::GetInstance();
                device.DestroyDescriptorSet(descriptorSet);
                device.GetDevice().destroyDescriptorSetLayout(descriptorLayout);
            });
    }
} // namespace BeeEngine::Internal
