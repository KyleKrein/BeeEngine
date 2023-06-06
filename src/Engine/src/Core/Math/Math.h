//
// Created by alexl on 06.06.2023.
//

#pragma once

#include "glm.hpp"

namespace BeeEngine::Math
{
    struct DecomposedTransform
    {
        glm::vec3 Translation;
        glm::vec3 Rotation;
        glm::vec3 Scale;
    };

    DecomposedTransform DecomposeTransform(const glm::mat4& transform);
}