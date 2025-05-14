#pragma once
#include "Core/AssetManagement/Asset.h"
#include "Core/AssetManagement/EditorAssetManager.h"
#include "Core/TypeDefines.h"

namespace BeeEngine
{
    class RmlDocument final : public Asset
    {
    public:
        [[nodiscard]] constexpr AssetType GetType() const final { return AssetType::RmlDocument; }
        std::function<String()> GetFileContent;
    };
    class Rcss : public Asset
    {
    public:
        [[nodiscard]] constexpr AssetType GetType() const final { return AssetType::RcssStyle; }
        std::function<String()> GetFileContent;
    };
} // namespace BeeEngine
