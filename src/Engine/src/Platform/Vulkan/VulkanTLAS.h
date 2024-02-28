//
// Created by Aleksandr on 25.02.2024.
//

#pragma once
#include "Renderer/TopLevelAccelerationStructure.h"


namespace BeeEngine::Internal
{
    class VulkanTLAS final: public TopLevelAccelerationStructure
    {
    public:
        ~VulkanTLAS() override;

        void UpdateInstances(std::vector<SceneTreeRenderer::Entity>&& entities) final;
    };

}
