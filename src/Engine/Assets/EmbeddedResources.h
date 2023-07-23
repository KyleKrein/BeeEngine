//
// Created by alexl on 04.06.2023.
//

#pragma once

#include "gsl/gsl"

enum class EmbeddedResource
{
    OpenSansRegular = 0,
    OpenSansBold = 1,
    ManropeRegular = 2,
    ManropeBold = 3,
    DirectoryTexture = 100,
    FileTexture = 101,
    PlayButtonTexture = 102,
    PauseButtonTexture = 103,
    StopButtonTexture = 104,
    Standart2DShaderVertex = 200,
    Standart2DShaderFragment = 201
};

namespace BeeEngine::Internal
{
    gsl::span<std::byte> GetEmbeddedResource(EmbeddedResource resource) noexcept;
}
