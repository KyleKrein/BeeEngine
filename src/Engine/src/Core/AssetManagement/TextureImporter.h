//
// Created by alexl on 16.08.2023.
//

#pragma once
#include "AssetMetadata.h"
#include "Core/TypeDefines.h"
#include "Renderer/Texture.h"
#include <span>

namespace BeeEngine
{
    class TextureImporter
    {
    public:
        static Ref<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata);
        static Scope<GPUTextureResource> LoadTextureFromFile(const Path& path);
        static Scope<GPUTextureResource> LoadTextureFromMemory(std::span<byte> data);
    };
} // namespace BeeEngine
