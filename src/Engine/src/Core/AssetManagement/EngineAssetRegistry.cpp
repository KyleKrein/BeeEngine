//
// Created by alexl on 16.08.2023.
//

#include "EngineAssetRegistry.h"
#include "../../../Assets/EmbeddedResources.h"

namespace BeeEngine
{
    static void LoadTextures(IAssetManager* assetManager)
    {
        auto stopButtonTexture = Internal::GetEmbeddedResource(EmbeddedResource::StopButtonTexture);
        assetManager->LoadAsset(stopButtonTexture, EngineAssetRegistry::StopButtonTexture, "Stop Button Texture", AssetType::Texture2D);

        auto playButtonTexture = Internal::GetEmbeddedResource(EmbeddedResource::PlayButtonTexture);
        assetManager->LoadAsset(playButtonTexture, EngineAssetRegistry::PlayButtonTexture, "Play Button Texture", AssetType::Texture2D);

        auto directoryTexture = Internal::GetEmbeddedResource(EmbeddedResource::DirectoryTexture);
        assetManager->LoadAsset(directoryTexture, EngineAssetRegistry::DirectoryTexture, "Directory Texture", AssetType::Texture2D);

        auto fileTexture = Internal::GetEmbeddedResource(EmbeddedResource::FileTexture);
        assetManager->LoadAsset(fileTexture, EngineAssetRegistry::FileTexture, "File Texture", AssetType::Texture2D);
    }

    static void LoadFonts(IAssetManager* assetManager)
    {
        auto openSansRegular = Internal::GetEmbeddedResource(EmbeddedResource::OpenSansRegular);
        assetManager->LoadAsset(openSansRegular,EngineAssetRegistry::OpenSansRegular ,"OpenSans Regular", AssetType::Font);

        auto openSansBold = Internal::GetEmbeddedResource(EmbeddedResource::OpenSansBold);
        assetManager->LoadAsset(openSansBold, EngineAssetRegistry::OpenSansBold, "OpenSans Bold", AssetType::Font);

        auto manropeRegular = Internal::GetEmbeddedResource(EmbeddedResource::ManropeRegular);
        assetManager->LoadAsset(manropeRegular, EngineAssetRegistry::ManropeRegular, "Manrope Regular", AssetType::Font);

        auto manropeBold = Internal::GetEmbeddedResource(EmbeddedResource::ManropeBold);
        assetManager->LoadAsset(manropeBold, EngineAssetRegistry::ManropeBold, "Manrope Bold", AssetType::Font);
    }

    void EngineAssetRegistry::RegisterAssetTypes(IAssetManager *assetManager)
    {
        LoadFonts(assetManager);
        LoadTextures(assetManager);
    }
}
