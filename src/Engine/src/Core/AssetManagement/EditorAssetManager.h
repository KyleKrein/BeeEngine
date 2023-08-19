//
// Created by alexl on 16.08.2023.
//

#pragma once
#include "IAssetManager.h"
namespace BeeEngine
{
    class EditorAssetManager final: public IAssetManager
    {
    public:
        EditorAssetManager();
        [[nodiscard]] Ref<Asset> GetAssetRef(AssetHandle handle) const final;

        Asset* GetAsset(AssetHandle handle) const final;

        const AssetHandle*  GetAssetHandleByName(std::string_view name) const;

        void LoadAsset(gsl::span<byte> data, AssetHandle handle, const std::string& name, AssetType type) final;

        void LoadAsset(const std::filesystem::path &path, AssetHandle handle) final;

        void UnloadAsset(AssetHandle handle) final;

        [[nodiscard]] bool IsAssetHandleValid(AssetHandle handle) const final;

        [[nodiscard]] bool IsAssetLoaded(AssetHandle handle) const final;

        [[nodiscard]] AssetRegistry& GetAssetRegistry() final { return m_AssetRegistry; }

    private:
        mutable AssetMap m_AssetMap;
        AssetRegistry m_AssetRegistry;
        std::map<std::string, AssetHandle> m_AssetNameMap;
    };
}
