//
// Created by alexl on 16.08.2023.
//

#include "TextureImporter.h"
#include "Core/AssetManagement/Asset.h"
#include "Core/CodeSafety/Expects.h"
#include "Core/ScopeGuard.h"
#include "Renderer/Texture.h"
#if defined(WINDOWS)
#define STBI_WINDOWS_UTF8
#include "Platform/Windows/WindowsString.h"
#endif
#include <stb/stb_image.h>
#if defined(WINDOWS)
STBIDEF stbi_uc* stbi_load(const wchar_t* filename, int* x, int* y, int* comp, int req_comp);
#endif

namespace BeeEngine
{
    Scope<GPUTextureResource> TextureImporter::LoadTextureFromFile(const Path& filepath)
    {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
#if defined(WINDOWS)
        std::wstring path = Internal::WStringFromUTF8(filepath);
#else
        const String& path = filepath;
#endif
        stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        ScopeGuard guard{[data]()
                         {
                             if (data)
                             {
                                 stbi_image_free(data);
                             }
                         }};
        if (!data)
        {
            return nullptr;
        }
        return GPUTextureResource::Create(width, height, {(byte*)data, size_t(width * height * channels)}, channels);
    }
    Scope<GPUTextureResource> TextureImporter::LoadTextureFromMemory(gsl::span<byte> textureData)
    {
        BeeExpects(!textureData.empty());
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        stbi_uc* data = stbi_load_from_memory(reinterpret_cast<stbi_uc*>(textureData.data()),
                                              gsl::narrow_cast<int>(textureData.size()),
                                              &width,
                                              &height,
                                              &channels,
                                              0);
        ScopeGuard guard{[data]()
                         {
                             if (data)
                             {
                                 stbi_image_free(data);
                             }
                         }};
        if (!data)
        {
            return nullptr;
        }
        return GPUTextureResource::Create(width, height, {(byte*)data, size_t(width * height * channels)}, channels);
    }

    Ref<Texture2D> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata)
    {
        Ref<Texture2D> result = [&metadata]()
        {
            switch (metadata.Location)
            {
                case AssetLocation::FileSystem:
                    return CreateRef<Texture2D>(LoadTextureFromFile(std::get<Path>(metadata.Data)));
                case AssetLocation::Embedded:
                    return CreateRef<Texture2D>(LoadTextureFromMemory(std::get<gsl::span<byte>>(metadata.Data)));
            }
            BeeEnsures(false && "Unknown location of asset");
        }();
        if (!result)
        {
            BeeCoreError("Failed to load image: {0}",
                         metadata.Location == AssetLocation::FileSystem ? std::get<Path>(metadata.Data).AsCString()
                                                                        : "from memory");
            return nullptr;
        }
        result->Handle = handle;
        result->Location = metadata.Location;
        return result;
    }
} // namespace BeeEngine
