//
// Created by alexl on 16.08.2023.
//

#include "EngineAssetRegistry.h"
#include "../../../Assets/EmbeddedResources.h"

namespace BeeEngine
{
    consteval static auto CreateCheckerboardTexture()
    {
        uint32_t magenta = 0xFF00FFFF;
        uint32_t black = 0x000000FF;
        std::array<uint32_t, 16 * 16> pixels; // for 16x16 checkerboard texture
        for (int x = 0; x < 16; x++)
        {
            for (int y = 0; y < 16; y++)
            {
                pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
            }
        }
        return pixels;
    }
    static void LoadTextures(IAssetManager* assetManager)
    {
        auto stopButtonTexture = Internal::GetEmbeddedResource(EmbeddedResource::StopButtonTexture);
        assetManager->LoadAsset(
            stopButtonTexture, EngineAssetRegistry::StopButtonTexture, "Stop Button Texture", AssetType::Texture2D);

        auto playButtonTexture = Internal::GetEmbeddedResource(EmbeddedResource::PlayButtonTexture);
        assetManager->LoadAsset(
            playButtonTexture, EngineAssetRegistry::PlayButtonTexture, "Play Button Texture", AssetType::Texture2D);

        auto directoryTexture = Internal::GetEmbeddedResource(EmbeddedResource::DirectoryTexture);
        assetManager->LoadAsset(
            directoryTexture, EngineAssetRegistry::DirectoryTexture, "Directory Texture", AssetType::Texture2D);

        auto fileTexture = Internal::GetEmbeddedResource(EmbeddedResource::FileTexture);
        assetManager->LoadAsset(fileTexture, EngineAssetRegistry::FileTexture, "File Texture", AssetType::Texture2D);

        constexpr static std::array<uint32_t, 16 * 16> pixels =
            CreateCheckerboardTexture(); // for 16x16 checkerboard texture
        // assetManager->LoadAsset({(byte*)pixels.data(), pixels.size() * sizeof(uint32_t)},
        // EngineAssetRegistry::CheckerboardTexture, "Checkerboard Texture", AssetType::Texture2D);
    }

    static void LoadFonts(IAssetManager* assetManager)
    {
        auto openSansRegular = Internal::GetEmbeddedResource(EmbeddedResource::OpenSansRegular);
        assetManager->LoadAsset(
            openSansRegular, EngineAssetRegistry::OpenSansRegular, "OpenSans Regular", AssetType::Font);

        auto openSansBold = Internal::GetEmbeddedResource(EmbeddedResource::OpenSansBold);
        assetManager->LoadAsset(openSansBold, EngineAssetRegistry::OpenSansBold, "OpenSans Bold", AssetType::Font);

        auto manropeRegular = Internal::GetEmbeddedResource(EmbeddedResource::ManropeRegular);
        assetManager->LoadAsset(
            manropeRegular, EngineAssetRegistry::ManropeRegular, "Manrope Regular", AssetType::Font);

        auto manropeBold = Internal::GetEmbeddedResource(EmbeddedResource::ManropeBold);
        assetManager->LoadAsset(manropeBold, EngineAssetRegistry::ManropeBold, "Manrope Bold", AssetType::Font);
    }

    void EngineAssetRegistry::RegisterAssetTypes(IAssetManager* assetManager)
    {
        LoadFonts(assetManager);
        LoadTextures(assetManager);
    }
} // namespace BeeEngine
