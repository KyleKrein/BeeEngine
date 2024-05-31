//
// Created by alexl on 16.08.2023.
//

#pragma once
#include "AssetMetadata.h"
#include "Core/TypeDefines.h"
#include "Renderer/Mesh.h"

namespace BeeEngine
{
    class MeshImporter
    {
    public:
        static Ref<Mesh> ImportMesh(AssetHandle handle, const AssetMetadata& metadata);
    };
} // namespace BeeEngine
