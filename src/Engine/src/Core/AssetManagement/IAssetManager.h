//
// Created by alexl on 16.08.2023.
//

#pragma once
#include "Asset.h"
#include "gsl/span"
#include "AssetMetadata.h"
#include "Core/Path.h"
#include <map>
#include <filesystem>

namespace BeeEngine
{
    using AssetMap = std::map<AssetHandle, Ref<Asset>>;
    using AssetRegistry = std::map<UUID, std::map<UUID,AssetMetadata>>;
    class IAssetManager
    {
    public:
        virtual Ref<Asset> GetAssetRef(AssetHandle handle) const = 0;
        virtual Asset * GetAsset(AssetHandle handle) const = 0;
        virtual void LoadAsset(gsl::span<byte> data, AssetHandle handle, const std::string& name, AssetType type) = 0;
        virtual void LoadAsset(const Path& path, AssetHandle handle) = 0;
        virtual void UnloadAsset(AssetHandle handle) = 0;

        virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
        virtual bool IsAssetLoaded(AssetHandle handle) const = 0;

        virtual AssetRegistry& GetAssetRegistry() = 0;
    };
}
