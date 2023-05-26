//
// Created by alexl on 14.05.2023.
//

#pragma once

#include "Core/Color4.h"

namespace BeeEngine
{
    struct RectangleProperties
    {
        float X = 0, Y = 0, Z = 0, Width = 1, Height = 1;
        float Rotation = 0;
        Color4 Color = Color4::White;
        constexpr RectangleProperties() = default;
        constexpr RectangleProperties(float x, float y, float z, float width, float height, float rotation, Color4 color)
            : X(x), Y(y), Z(z), Width(width), Height(height), Rotation(rotation), Color(color)
        {
        }

        constexpr RectangleProperties(float x, float y, float width, float height, float rotation, Color4 color)
            : X(x), Y(y), Z(0), Width(width), Height(height), Rotation(rotation), Color(color)
        {
        }
    };
}
