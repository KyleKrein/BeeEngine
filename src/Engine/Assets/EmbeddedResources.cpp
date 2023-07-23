//
// Created by alexl on 07.06.2023.
//

#include "EmbeddedResources.h"
#define INCBIN_SILENCE_BITCODE_WARNING
#include "incbin.h"

#define EmbedResource(name, path) extern "C" {INCBIN(name, ASSETS_PATH path); }

EmbedResource(OpenSansRegular, "Fonts/OpenSans/static/OpenSans-Regular.ttf");
EmbedResource(OpenSansBold, "Fonts/OpenSans/static/OpenSans-Bold.ttf");
EmbedResource(ManropeRegular, "Fonts/Manrope/static/Manrope-Regular.ttf");
EmbedResource(ManropeBold, "Fonts/Manrope/static/Manrope-Bold.ttf");
EmbedResource(DirectoryTexture, "Textures/directory.png");
EmbedResource(FileTexture, "Textures/file.png");
EmbedResource(PlayButtonTexture, "Textures/PlayButton.png");
EmbedResource(PauseButtonTexture, "Textures/PauseButton.png");
EmbedResource(StopButtonTexture, "Textures/StopButton.png");
EmbedResource(Standart2DShaderVertex, "Shaders/Standart2DVertex.glsl");
EmbedResource(Standart2DShaderFragment, "Shaders/Standart2DFragment.glsl");
namespace BeeEngine::Internal
{
    gsl::span<std::byte> GetEmbeddedResource(EmbeddedResource resource) noexcept
    {
        switch (resource)
        {
            case EmbeddedResource::OpenSansRegular:
                return {(std::byte*)gOpenSansRegularData, gOpenSansRegularSize};
            case EmbeddedResource::OpenSansBold:
                return {(std::byte*)gOpenSansBoldData, gOpenSansBoldSize};
            case EmbeddedResource::ManropeRegular:
                return {(std::byte*)gManropeRegularData, gManropeRegularSize};
            case EmbeddedResource::ManropeBold:
                return {(std::byte*)gManropeBoldData, gManropeBoldSize};
            case EmbeddedResource::DirectoryTexture:
                return {(std::byte*)gDirectoryTextureData, gDirectoryTextureSize};
            case EmbeddedResource::FileTexture:
                return {(std::byte*)gFileTextureData, gFileTextureSize};
            case EmbeddedResource::PlayButtonTexture:
                return {(std::byte*)gPlayButtonTextureData, gPlayButtonTextureSize};
            case EmbeddedResource::PauseButtonTexture:
                return {(std::byte*)gPauseButtonTextureData, gPauseButtonTextureSize};
            case EmbeddedResource::StopButtonTexture:
                return {(std::byte*)gStopButtonTextureData, gStopButtonTextureSize};
            case EmbeddedResource::Standart2DShaderVertex:
                return {(std::byte*)gStandart2DShaderVertexData, gStandart2DShaderVertexSize};
            case EmbeddedResource::Standart2DShaderFragment:
                return {(std::byte*)gStandart2DShaderFragmentData, gStandart2DShaderFragmentSize};
            default:
                return {};
        }
    }
}

