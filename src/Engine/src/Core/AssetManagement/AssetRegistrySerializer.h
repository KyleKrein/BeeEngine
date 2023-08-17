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
        AssetRegistrySerializer(IAssetManager* assetManager, const std::filesystem::path& projectPath)
            : m_AssetManager(assetManager), m_ProjectPath(projectPath)
        {}

        void Serialize(const std::filesystem::path& path);
        void Deserialize(const std::filesystem::path& path);
    private:
        IAssetManager* m_AssetManager;
        std::filesystem::path m_ProjectPath;
    };
}