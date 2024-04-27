//
// Created by Aleksandr on 25.02.2024.
//

#include "VulkanTLAS.h"

#include <glm.hpp>
#include <gtc/type_ptr.hpp>


namespace BeeEngine::Internal
{
    VulkanTLAS::VulkanTLAS()
        : m_Device(VulkanGraphicsDevice::GetInstance())
    {
    }

    VulkanTLAS::~VulkanTLAS()
    {
    }

    void VulkanTLAS::UpdateInstances(std::vector<SceneTreeRenderer::Entity>&& entities)
    {
    }

    std::vector<vk::AccelerationStructureInstanceKHR> VulkanTLAS::GenerateInstancedData(
        std::vector<SceneTreeRenderer::Entity>&& entities)
    {
        std::vector<vk::AccelerationStructureInstanceKHR> instances;
        instances.reserve(entities.size());
        for (auto& entity : entities)
        {
            vk::AccelerationStructureInstanceKHR instance;
            glm::mat4x3 transform = glm::transpose(entity.Transform);
            memcpy(&instance.transform, glm::value_ptr(transform), sizeof(instance.transform));
            //instance.instanceCustomIndex = entity.instanceCustomIndex;
            //instance.instanceShaderBindingTableRecordOffset = entity.instanceShaderBindingTableRecordOffset;
            //instance.mask = entity.mask;
            //instance.flags = entity.flags;
            //instance.accelerationStructureReference = entity.accelerationStructureReference;
            instances.push_back(instance);
        }
        return instances;
    }
}
