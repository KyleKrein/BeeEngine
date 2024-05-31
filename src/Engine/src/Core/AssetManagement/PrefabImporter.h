//
// Created by alexl on 16.10.2023.
//

#pragma once

#include "Asset.h"
#include "AssetMetadata.h"
#include "Scene/Entity.h"

namespace BeeEngine
{
    class PrefabImporter
    {
    public:
        static Ref<Asset> ImportPrefab(AssetHandle handle, const AssetMetadata& metadata);
        static Ref<Asset> GeneratePrefab(Entity rootEntity, const Path& path, const AssetHandle& handle);
    };
} // namespace BeeEngine
