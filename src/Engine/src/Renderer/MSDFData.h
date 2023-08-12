//
// Created by alexl on 12.08.2023.
//

#pragma once
#include <vector>
#include <msdf-atlas-gen/msdf-atlas-gen.h>
namespace BeeEngine::Internal
{
    struct MSDFData
    {
        std::vector<msdf_atlas::GlyphGeometry> Glyphs;
        msdf_atlas::FontGeometry FontGeometry;
    };
}