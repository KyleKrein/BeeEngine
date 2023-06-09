//
// Created by alexl on 09.06.2023.
//

#pragma once

#include <optional>
#include "Core/TypeDefines.h"

namespace BeeEngine
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> GraphicsFamily;
        std::optional<uint32_t> PresentFamily;

        bool IsComplete() const
        {
            return GraphicsFamily.has_value() && PresentFamily.has_value();
        }
    };
}
