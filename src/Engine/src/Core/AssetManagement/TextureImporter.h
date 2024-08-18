//
// Created by alexl on 16.08.2023.
//

#pragma once
#include "AssetMetadata.h"
#include "Core/TypeDefines.h"
#include "Renderer/Texture.h"

namespace BeeEngine
{
    class TextureImporter
    {
    public:
        static Ref<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata);
        static Ref<Texture2D> LoadTextureFromFile(const Path& path);
        static Ref<Texture2D> LoadTextureFromMemory(gsl::span<byte> data);
    };
} // namespace BeeEngine
