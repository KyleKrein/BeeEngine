//
// Created by alexl on 16.08.2023.
//

#include "AssetImporter.h"
#include "TextureImporter.h"
#include "FontImporter.h"
#include <map>

namespace BeeEngine
{
    using AssetImportFunction = std::function<Ref<Asset>(AssetHandle, const AssetMetadata&)>;
    static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions = {
            { AssetType::Texture2D, TextureImporter::ImportTexture2D },
            {AssetType::Font, FontImporter::ImportFont}
    };

    Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
    {
        if (!s_AssetImportFunctions.contains(metadata.Type))
        {
            BeeCoreError("No importer available for asset type: {}", (uint16_t)metadata.Type);
            return nullptr;
        }

        return s_AssetImportFunctions.at(metadata.Type)(handle, metadata);
    }
}
