//
// Created by alexl on 16.08.2023.
//

#include "MeshImporter.h"


namespace BeeEngine
{
    Ref<Mesh> MeshImporter::ImportMesh(AssetHandle handle, const AssetMetadata& metadata)
    {
        BeeExpects(metadata.Type == AssetType::Mesh);
        //throw std::exception("Not implemented");
        return nullptr;
    }
}
