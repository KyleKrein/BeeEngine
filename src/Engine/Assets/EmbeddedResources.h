//
// Created by alexl on 04.06.2023.
//

#pragma once
#include <span>

enum class EmbeddedResource
{
    OpenSansRegular = 0,
    OpenSansBold = 1,
    ManropeRegular = 2,
    ManropeBold = 3,
    BeeEngineLogo = 100,
    DirectoryTexture = 101,
    FileTexture = 102,
    PlayButtonTexture = 103,
    PauseButtonTexture = 104,
    StopButtonTexture = 105,
    Standart2DShaderVertex = 200,
    Standart2DShaderFragment = 201
};

namespace BeeEngine::Internal
{
    std::span<std::byte> GetEmbeddedResource(EmbeddedResource resource) noexcept;
}
