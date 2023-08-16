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
        [[nodiscard]] Ref<Asset> GetAsset(AssetHandle handle) const final;

        void LoadAsset(gsl::span<byte> data, AssetHandle handle, const std::string& name, AssetType type) final;

        void LoadAsset(const std::filesystem::path &path, AssetHandle handle) final;

        void UnloadAsset(AssetHandle handle) final;

        [[nodiscard]] bool IsAssetHandleValid(AssetHandle handle) const final;

        [[nodiscard]] bool IsAssetLoaded(AssetHandle handle) const final;

    private:
        mutable AssetMap m_AssetMap;
        AssetRegistry m_AssetRegistry;
    };
}
