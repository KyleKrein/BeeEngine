//
// Created by alexl on 16.08.2023.
//

#pragma once
#include "IAssetManager.h"
namespace BeeEngine
{
    class EditorAssetManager final : public IAssetManager
    {
    public:
        EditorAssetManager();
        [[nodiscard]] Ref<Asset> GetAssetRef(AssetHandle handle) const final;

        Asset* GetAsset(AssetHandle handle) const final;

        AssetMetadata& GetAssetMetadata(const AssetHandle& handle);

        const AssetHandle* GetAssetHandleByName(const String& name) const;

        void LoadAsset(gsl::span<byte> data, AssetHandle handle, const String& name, AssetType type) final;

        void LoadAsset(const Path& path, AssetHandle handle) final;

        void UnloadAsset(AssetHandle handle) final;

        [[nodiscard]] bool IsAssetHandleValid(AssetHandle handle) const final;

        [[nodiscard]] bool IsAssetLoaded(AssetHandle handle) const final;

        [[nodiscard]] AssetRegistry& GetAssetRegistry() final { return m_AssetRegistry; }

        void RemoveAsset(AssetHandle handle);

    private:
        mutable AssetMap m_AssetMap;
        AssetRegistry m_AssetRegistry;
        std::map<String, AssetHandle> m_AssetNameMap;
    };
} // namespace BeeEngine
