//
// Created by alexl on 16.08.2023.
//

#include "MeshImporter.h"

namespace BeeEngine
{
    Ref<Mesh> MeshImporter::ImportMesh(AssetHandle handle, const AssetMetadata& metadata)
    {
        BeeExpects(metadata.Type == AssetType::Mesh);
        /*if(metadata.Location == AssetLocation::FileSystem)
        {
            auto& path = std::get<Path>(metadata.Data);
            auto format = GetMeshFormat(path);
            BeeExpects(format != MeshFormat::Unknown);
            switch(format)
            {
                case MeshFormat::GLTF:
                    //return ImportGLTFMesh(handle, path);
                case MeshFormat::GLTF_BINARY:
                    //return ImportGLTFBinaryMesh(handle, path);
                case MeshFormat::Unknown:
                    BeeCoreError("Unknown mesh format for file: {}", path);
                    break;
            }
        }*/
        return nullptr;
    }
}
