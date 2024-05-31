//
// Created by Aleksandr on 02.04.2024.
//

#pragma once
#include "AssetMetadata.h"
#include "Core/TypeDefines.h"
#include "MeshSource.h"

namespace BeeEngine
{
    class MeshSourceImporter
    {
    public:
        static Ref<MeshSource> ImportMeshSource(AssetHandle handle, const AssetMetadata& metadata);
    };
} // namespace BeeEngine
