//
// Created by alexl on 16.08.2023.
//

#pragma once

#include "IAssetManager.h"

namespace BeeEngine
{
    class AssetRegistrySerializer
    {
    public:
        AssetRegistrySerializer(IAssetManager* assetManager)
            : m_AssetManager(assetManager)
        {}

        void Serialize(const std::filesystem::path& path);
        void Deserialize(const std::filesystem::path& path);
    private:
        IAssetManager* m_AssetManager;
    };
}