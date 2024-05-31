//
// Created by alexl on 16.08.2023.
//

#include "FontImporter.h"

namespace BeeEngine
{
    Ref<Font> FontImporter::ImportFont(AssetHandle handle, const AssetMetadata& metadata)
    {
        BeeExpects(metadata.Type == AssetType::Font);
        Ref<Font> result;
        if (metadata.Location == AssetLocation::FileSystem)
        {
            result = CreateRef<Font>(std::get<Path>(metadata.Data));
        }
        else
        {
            result = CreateRef<Font>(metadata.Name, std::get<gsl::span<byte>>(metadata.Data));
        }
        result->Handle = handle;
        result->Location = metadata.Location;
        return result;
    }
} // namespace BeeEngine
