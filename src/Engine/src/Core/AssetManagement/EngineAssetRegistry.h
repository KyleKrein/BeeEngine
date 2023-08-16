//
// Created by alexl on 16.08.2023.
//

#pragma once

#include "IAssetManager.h"
#include "Asset.h"

namespace BeeEngine
{
    class EngineAssetRegistry
    {
    public:
        static void RegisterAssetTypes(IAssetManager* assetManager);

        static constexpr AssetHandle OpenSansRegular = 328465234592345234;
        static constexpr AssetHandle OpenSansBold = 34564386543524541;

        static constexpr AssetHandle ManropeRegular = 786587689586932544;
        static constexpr AssetHandle ManropeBold = 54646863746848;

        static constexpr AssetHandle StopButtonTexture = 52345743905231234;
        static constexpr AssetHandle PlayButtonTexture = 899864344847468468;
        static constexpr AssetHandle DirectoryTexture = 849648416847684646;
        static constexpr AssetHandle FileTexture = 8496488484431620020;
    };
}
