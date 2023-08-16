//
// Created by alexl on 16.08.2023.
//

#pragma once
#include <filesystem>
#include "Asset.h"
#include "Core/TypeDefines.h"
#include "gsl/span"

namespace BeeEngine
{
    struct AssetMetadata
    {
        std::string Name;
        AssetType Type = AssetType::None;
        AssetLocation Location = AssetLocation::FileSystem;
        gsl::span<byte> Data;
        std::filesystem::path FilePath;

        operator bool() const { return Type != AssetType::None; }
    };
}