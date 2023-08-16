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
            std::string path = metadata.FilePath.string();
            data = stbi_load(path.c_str(), &width, &height, &channels, 4);
        }
        else
        {
            BeeExpects(!metadata.Data.empty());
            data = stbi_load_from_memory(reinterpret_cast<stbi_uc*>(metadata.Data.data()), gsl::narrow_cast<int>(metadata.Data.size()), &width, &height, &channels, 4);
        }
        if (!data)
        {
            BeeCoreError("Failed to load image: {0}", metadata.Location == AssetLocation::FileSystem ? metadata.FilePath.string() : "from memory");
            return nullptr;
        }
        if (channels == 3)
        {
            BeeCoreTrace("Converting image from RGB to RGBA");
            // Add alpha channel
            auto *dataWithAlpha = new unsigned char[width * height * 4];
            for (int i = 0; i < width * height; ++i)
            {
                dataWithAlpha[i * 4] = data[i * 3];
                dataWithAlpha[i * 4 + 1] = data[i * 3 + 1];
                dataWithAlpha[i * 4 + 2] = data[i * 3 + 2];
                dataWithAlpha[i * 4 + 3] = 255;
            }
            stbi_image_free(data);
            data = dataWithAlpha;
        }

        auto result =  Texture2D::Create(width, height, {(byte*)data, size_t(width * height * 4)});
        result->Handle = handle;
        result->Location = metadata.Location;
        if (channels == 3)
        {
            delete[] data;
        }
        else
        {
            stbi_image_free(data);
        }
        return result;
    }
}
