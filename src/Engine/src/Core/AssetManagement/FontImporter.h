//
// Created by alexl on 16.08.2023.
//

#pragma once
#include "AssetMetadata.h"
#include "Renderer/Font.h"

namespace BeeEngine
{
    class FontImporter
    {
    public:
        static Ref<Font> ImportFont(AssetHandle handle, const AssetMetadata& metadata);
    };
} // namespace BeeEngine
