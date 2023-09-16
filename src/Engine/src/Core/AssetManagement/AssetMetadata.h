//
// Created by alexl on 16.08.2023.
//

#pragma once
#include <filesystem>
#include "Asset.h"
#include "Core/TypeDefines.h"
#include "gsl/span"
#include "Core/Path.h"
#include <variant>

namespace BeeEngine
{
    struct AssetMetadata
    {
        String Name;
        AssetType Type = AssetType::None;
        AssetLocation Location = AssetLocation::FileSystem;
        std::variant<gsl::span<byte>, Path> Data;

        operator bool() const { return Type != AssetType::None; }
    };
}