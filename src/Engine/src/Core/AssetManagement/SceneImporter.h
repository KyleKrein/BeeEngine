#pragma once

#include "Core/AssetManagement/AssetMetadata.h"
#include "Scene/Scene.h"
namespace BeeEngine
{
    class SceneImporter
    {
    public:
        static Ref<Scene> ImportScene(AssetHandle handle, const AssetMetadata& metadata);
    };
} // namespace BeeEngine