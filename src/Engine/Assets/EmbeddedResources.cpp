//
// Created by alexl on 07.06.2023.
//

#include "EmbeddedResources.h"

#include "incbin.h"

#define EmbedResource(name, path) extern "C" {INCBIN(name, ASSETS_PATH "" path); }

EmbedResource(OpenSansRegular, "OpenSans/static/OpenSans-Regular.ttf");
EmbedResource(OpenSansBold, "OpenSans/static/OpenSans-Bold.ttf");
EmbedResource(DirectoryTexture, "Textures/directory.png");
EmbedResource(FileTexture, "Textures/file.png");
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
            case EmbeddedResource::DirectoryTexture:
                return {(std::byte*)gDirectoryTextureData, gDirectoryTextureSize};
            case EmbeddedResource::FileTexture:
                return {(std::byte*)gFileTextureData, gFileTextureSize};
            default:
                return {};
        }
    }
}

