//
// Created by alexl on 16.08.2023.
//

#include "TextureImporter.h"
#include <stb_image.h>


namespace BeeEngine
{

    Ref<Texture2D> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata &metadata)
    {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        stbi_uc* data = nullptr;
        if(metadata.Location == AssetLocation::FileSystem)
        {
            String path = std::get<Path>(metadata.Data);
            data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        }
        else
        {
            BeeExpects(!std::get<gsl::span<byte>>(metadata.Data).empty());
            data = stbi_load_from_memory(reinterpret_cast<stbi_uc*>(std::get<gsl::span<byte>>(metadata.Data).data()), gsl::narrow_cast<int>(std::get<gsl::span<byte>>(metadata.Data).size()), &width, &height, &channels, 0);
        }
        if (!data)
        {
            BeeCoreError("Failed to load image: {0}", metadata.Location == AssetLocation::FileSystem ? std::get<Path>(metadata.Data).AsCString() : "from memory");
            return nullptr;
        }

        auto result =  Texture2D::Create(width, height, {(byte*)data, size_t(width * height * channels)}, channels);
        result->Handle = handle;
        result->Location = metadata.Location;

        stbi_image_free(data);
        return result;
    }
}
