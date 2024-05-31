//
// Created by alexl on 09.06.2023.
//

#pragma once

#include "Core/TypeDefines.h"
#include <optional>

namespace BeeEngine
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> GraphicsFamily;
        std::optional<uint32_t> PresentFamily;

        bool IsComplete() const { return GraphicsFamily.has_value() && PresentFamily.has_value(); }
    };
} // namespace BeeEngine
