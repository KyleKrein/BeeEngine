//
// Created by Александр Лебедев on 25.06.2023.
//

#pragma once

#include "glm.hpp"

namespace BeeEngine
{
    struct Vertex
    {
        glm::vec3 Position {
                0.0f, 0.0f, 0.0f
        };
        glm::vec3 Normal {
                0.0f, 0.0f, 0.0f
        };
        glm::vec3 Color {
                0.0f, 0.0f, 0.0f
        };
    };
}