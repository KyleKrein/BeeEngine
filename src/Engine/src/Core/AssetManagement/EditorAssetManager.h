//
// Created by alexl on 16.08.2023.
//

#pragma once
#include "Core/AssetManagement/Asset.h"
#include "Core/Coroutines/Generator.h"
#include "IAssetManager.h"
#include <unordered_map>
#include <vector>
namespace BeeEngine
{
    class EditorAssetManager final : public IAssetManager
    {
    public:
        EditorAssetManager();
        [[nodiscard]] Ref<Asset> GetAssetRef(AssetHandle handle) const final;

        Asset* GetAsset(AssetHandle handle) const final;

        AssetMetadata& GetAssetMetadata(const AssetHandle& handle);

        [[deprecated("If two assets have the same name, this method returns only the last loaded. This behavior is "
                     "incorrect, therefore this method must be replaced")]] const AssetHandle*
        GetAssetHandleByName(
            const String& name) const; // TODO write the replacement, that works on path, not on the name

        void LoadAsset(std::span<byte> data, AssetHandle handle, const String& name, AssetType type) final;

        void LoadAsset(const Path& path, AssetHandle handle) final;

        void UnloadAsset(AssetHandle handle) final;

        [[nodiscard]] bool IsAssetHandleValid(AssetHandle handle) const final;

        [[nodiscard]] bool IsAssetLoaded(AssetHandle handle) const final;

        [[nodiscard]] AssetRegistry& GetAssetRegistry() final { return m_AssetRegistry; }

        void RemoveAsset(AssetHandle handle);

        std::span<const AssetHandle> GetAssetHandlesByType(AssetType type) const;

        Generator<Asset&> GetAssetsOfType(AssetType type) const;
        Generator<Asset&> IterateAssets() const;
        Generator<std::pair<AssetHandle, const AssetMetadata*>> GetAssetsDataOfType(AssetType type) const;
        Generator<std::pair<AssetHandle, const AssetMetadata*>> IterateAssetsData() const;

    private:
        mutable AssetMap m_AssetMap;
        AssetRegistry m_AssetRegistry;
        std::map<String, AssetHandle> m_AssetNameMap;
        std::unordered_map<AssetType, std::vector<AssetHandle>> m_TypeMap;
    };
} // namespace BeeEngine
