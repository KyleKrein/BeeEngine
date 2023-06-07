//
// Created by alexl on 04.06.2023.
//

#pragma once

#include "gsl/gsl"

enum class EmbeddedResource
{
    OpenSansRegular,
    OpenSansBold,
    DirectoryTexture,
    FileTexture,
    Standart2DShaderVertex,
    Standart2DShaderFragment
};

namespace BeeEngine::Internal
{
    gsl::span<std::byte> GetEmbeddedResource(EmbeddedResource resource) noexcept;
}
