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
        AssetRegistrySerializer(IAssetManager* assetManager, const Path& projectPath, UUID projectRegistryID)
            : m_AssetManager(assetManager), m_ProjectPath(projectPath), m_ProjectRegistryID(projectRegistryID)
        {}

        void Serialize(const Path& path);
        void Deserialize(const Path& path);
    private:
        IAssetManager* m_AssetManager;
        Path m_ProjectPath;
        UUID m_ProjectRegistryID;
    };
}