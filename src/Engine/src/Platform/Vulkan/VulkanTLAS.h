//
// Created by Aleksandr on 25.02.2024.
//

#pragma once
#include "Renderer/TopLevelAccelerationStructure.h"
#include <vulkan/vulkan.hpp>

#include "VulkanGraphicsDevice.h"

namespace BeeEngine::Internal
{
    class VulkanTLAS final: public TopLevelAccelerationStructure
    {
    public:
        VulkanTLAS();
        ~VulkanTLAS() override;

        void UpdateInstances(std::vector<SceneTreeRenderer::Entity>&& entities) final;
    private:
        std::vector<vk::AccelerationStructureInstanceKHR> GenerateInstancedData(std::vector<SceneTreeRenderer::Entity>&& entities);
    private:
        VulkanGraphicsDevice& m_Device;
        vk::AccelerationStructureKHR m_TLAS;
    };

}
