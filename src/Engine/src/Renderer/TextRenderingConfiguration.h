//
// Created by alexl on 13.08.2023.
//

#pragma once

#include "Core/Color4.h"

namespace BeeEngine
{
    struct TextRenderingConfiguration
    {
        Color4 ForegroundColor = Color4::Black;
        Color4 BackgroundColor = Color4::Transparent;
        float KerningOffset = 0.0f;
        float LineSpacing = 0.0f;
        template<typename Archive>
        void Serialize(Archive& serializer)
        {
            serializer & ForegroundColor;
            serializer & BackgroundColor;
            serializer & KerningOffset;
            serializer & LineSpacing;
        }
    };
}