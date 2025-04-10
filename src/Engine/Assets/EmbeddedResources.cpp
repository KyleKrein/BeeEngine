//
// Created by alexl on 07.06.2023.
//

#include "EmbeddedResources.h"
#if 0 //! defined(_MSC_VER)
#define INCBIN_SILENCE_BITCODE_WARNING
#include "incbin.h"

#define EmbedResource(name, path)                                                                                      \
    extern "C"                                                                                                         \
    {                                                                                                                  \
        INCBIN(name, ASSETS_PATH path);                                                                                \
    }

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

#else
extern "C"
{
#include <BeeEngineLogo_png.h>
#include <Manrope_Bold_ttf.h>
#include <Manrope_Regular_ttf.h>
#include <OpenSans_Bold_ttf.h>
#include <OpenSans_Regular_ttf.h>
#include <PauseButton_png.h>
#include <PlayButton_png.h>
#include <SimulateButton_png.h>
#include <Standart2DFragment_glsl.h>
#include <Standart2DVertex_glsl.h>
#include <StopButton_png.h>
#include <directory_png.h>
#include <file_png.h>
}

namespace BeeEngine::Internal
{
    std::span<std::byte> GetEmbeddedResource(EmbeddedResource resource) noexcept
    {
        switch (resource)
        {
            case EmbeddedResource::OpenSansRegular:
                return {(std::byte*)OpenSans_Regular_ttf_data, OpenSans_Regular_ttf_size};
            case EmbeddedResource::OpenSansBold:
                return {(std::byte*)OpenSans_Bold_ttf_data, OpenSans_Bold_ttf_size};
            case EmbeddedResource::ManropeRegular:
                return {(std::byte*)Manrope_Regular_ttf_data, Manrope_Regular_ttf_size};
            case EmbeddedResource::ManropeBold:
                return {(std::byte*)Manrope_Bold_ttf_data, Manrope_Bold_ttf_size};
            case EmbeddedResource::BeeEngineLogo:
                return {(std::byte*)BeeEngineLogo_png_data, BeeEngineLogo_png_size};
            case EmbeddedResource::DirectoryTexture:
                return {(std::byte*)directory_png_data, directory_png_size};
            case EmbeddedResource::FileTexture:
                return {(std::byte*)file_png_data, file_png_size};
            case EmbeddedResource::PlayButtonTexture:
                return {(std::byte*)PlayButton_png_data, PlayButton_png_size};
            case EmbeddedResource::PauseButtonTexture:
                return {(std::byte*)PauseButton_png_data, PauseButton_png_size};
            case EmbeddedResource::StopButtonTexture:
                return {(std::byte*)StopButton_png_data, StopButton_png_size};
            case EmbeddedResource::Standart2DShaderVertex:
                return {(std::byte*)Standart2DVertex_glsl_data, Standart2DVertex_glsl_size};
            case EmbeddedResource::Standart2DShaderFragment:
                return {(std::byte*)Standart2DFragment_glsl_data, Standart2DFragment_glsl_size};
            default:
                return {};
        }
    }
} // namespace BeeEngine::Internal
#endif
