//
// Created by alexl on 16.08.2023.
//

#pragma once
#include "Renderer/Texture.h"
#include "AssetMetadata.h"
namespace BeeEngine
{
    class TextureImporter
    {
    public:
        static Ref<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata);
    };
}
