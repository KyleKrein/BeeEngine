//
// Created by Aleksandr on 10.03.2024.
//

#include "VulkanBindingSet.h"

#include "VulkanPipeline.h"
#include "Renderer/CommandBuffer.h"
#include "Renderer/IBindable.h"

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
            //case PipelineType::RayTracing:
            //return vk::PipelineBindPoint::eRayTracingKHR;
            default:
                return vk::PipelineBindPoint::eGraphics;
        }
    }

    VulkanBindingSet::VulkanBindingSet(std::initializer_list<BindingSetElement> elements)
        : BindingSet(elements), m_GraphicsDevice(VulkanGraphicsDevice::GetInstance())
    {
        std::vector<vk::DescriptorSetLayoutBinding> bindings;
        for (const auto& element : m_Elements)
        {
            auto binding = element.Data.GetBindGroupLayoutEntry();
            size_t bindingIndex = element.Binding;
            for (auto& entry : binding)
            {
                auto& vkEntry = std::get<vk::DescriptorSetLayoutBinding>(entry);
                vkEntry.binding = bindingIndex++;
                bindings.push_back(vkEntry);
            }
        }
        vk::DescriptorSetLayoutCreateInfo layoutInfo({}, (uint32_t)bindings.size(), bindings.data());
        m_DescriptorSetLayout = m_GraphicsDevice.GetDevice().createDescriptorSetLayout(layoutInfo);
        vk::DescriptorSetAllocateInfo allocInfo {};
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &m_DescriptorSetLayout;
        m_GraphicsDevice.CreateDescriptorSet(allocInfo, &m_DescriptorSet);
    }

    void VulkanBindingSet::Bind(void* cmd, uint32_t index) const
    {
        auto& currentPipeline = VulkanPipeline::GetCurrentPipeline();
        std::vector<vk::WriteDescriptorSet> descriptorWrites;
        for (const auto& element : m_Elements)
        {
            auto binding = element.Data.GetBindGroupEntry();
            size_t bindingIndex = element.Binding;
            for (auto& entry : binding)
            {
                auto& writeDescriptorSet = std::get<vk::WriteDescriptorSet>(entry);
                writeDescriptorSet.dstSet = m_DescriptorSet;
                writeDescriptorSet.dstBinding = bindingIndex++;
                writeDescriptorSet.dstArrayElement = 0;
                //writeDescriptorSet.descriptorType = vkEntry.descriptorType;
                //writeDescriptorSet.descriptorCount = vkEntry.descriptorCount;
                //writeDescriptorSet.pBufferInfo = vkEntry.pBufferInfo;
                //writeDescriptorSet.pImageInfo = vkEntry.pImageInfo;
                //writeDescriptorSet.pTexelBufferView = vkEntry.pTexelBufferView;
                descriptorWrites.push_back(writeDescriptorSet);
            }
        }
        m_GraphicsDevice.GetDevice().updateDescriptorSets(descriptorWrites, nullptr);

        auto commandBuffer = static_cast<CommandBuffer*>(cmd)->GetHandleAs<vk::CommandBuffer>();
        commandBuffer.bindDescriptorSets(GetPipelineBindPoint(currentPipeline.GetType()), currentPipeline.GetPipelineLayout(), index, 1, &m_DescriptorSet, 0, nullptr);
    }

    VulkanBindingSet::~VulkanBindingSet()
    {
        DeletionQueue::Frame().PushFunction([descriptorSet = m_DescriptorSet, descriptorLayout = m_DescriptorSetLayout]()
        {
            auto& device = VulkanGraphicsDevice::GetInstance();
            device.DestroyDescriptorSet(descriptorSet);
            device.GetDevice().destroyDescriptorSetLayout(descriptorLayout);
        });
    }
}
