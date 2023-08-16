//
// Created by alexl on 16.08.2023.
//

#pragma once

#include "Core/UUID.h"

namespace BeeEngine
{
    using AssetHandle = UUID;
    enum class AssetType: uint16_t
    {
        None = 0,
        Texture2D = 1,
        Font = 2,
        Shader = 3,
        Mesh = 4,
        Material = 5,
        Model = 6,
    };
    enum class AssetLocation: uint16_t
    {
        FileSystem = 0,
        Embedded = 1
    };
    struct Asset
    {
        AssetHandle Handle;
        AssetLocation Location = AssetLocation::FileSystem;
        virtual constexpr AssetType GetType() const = 0;

        virtual ~Asset() = default;
    };
}