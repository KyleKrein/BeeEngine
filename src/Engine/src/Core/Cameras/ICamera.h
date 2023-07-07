//
// Created by alexl on 14.05.2023.
//

#pragma once

#include "detail/type_mat4x4.hpp"

namespace BeeEngine
{
    class ICamera
    {

    public:
        [[nodiscard]] virtual const glm::mat4 & GetViewProjectionMatrix() const = 0;
        virtual ~ICamera() = default;
    };
}
