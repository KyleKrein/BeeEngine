//
// Created by Aleksandr on 25.02.2024.
//

#pragma once
#include "SceneTreeRenderer.h"

namespace BeeEngine
{
    class TopLevelAccelerationStructure
    {
    public:
        virtual ~TopLevelAccelerationStructure() = default;
        virtual void UpdateInstances(std::vector<BeeEngine::SceneTreeRenderer::Entity> &&entities) = 0;

        static Ref<TopLevelAccelerationStructure> Create();
    };
}
