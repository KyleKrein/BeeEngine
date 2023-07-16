//
// Created by alexl on 16.07.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include "Core/Color4.h"
#include "Core/ValueType.h"
#include <glm/glm.hpp>
namespace BeeEngine
{
    struct MaterialDescriptor
    {
        glm::vec4 BaseColor;
        float Metallic; // 0 or 1
        float Roughness; // 0.0f - 1.0f
        float Reflectance; // 0.0f - 1.0f prefer > 0.35f
        glm::vec3 SheenColor; // 0.0f - 1.0f
        float SheenRoughness; // 0.0f - 1.0f
        float ClearCoat; //0 or 1
        float ClearCoatRoughness; // 0.0f - 1.0f
        float Anisotropy; // -1.0f - 1.0f Anisotropy is in the tangent direction when this value is positive
        glm::vec3 AnisotropyDirection; // 0.0f - 1.0f Linear RGB, encodes a direction vector in tangent space
        float AmbientOcclusion; // 0.0f - 1.0f
        glm::vec3 Normal; // 0.0f - 1.0f Linear RGB, encodes a normal vector in tangent space
        glm::vec3 BentNormal; // 0.0f - 1.0f Linear RGB, encodes a normal vector in tangent space
        glm::vec3 ClearCoatNormal; // 0.0f - 1.0f Linear RGB, encodes a normal vector in tangent space
        glm::vec4 Emissive; // rgb = 0.0f - n, a = 0.0f - 1.0f. Linear RGB intensity in nits, alpha encodes the exposure weight
        glm::vec4 PostLightingColor; //0.0f - 1.0f Linear RGB
        float IOR; // 1.0f - n Optional, usually deduced from the reflectance
        float Transmission; // 0.0f - 1.0f
        glm::vec3 Absorption; // 0.0f - n
        float MicroThickness; // 0.0f - n
        float Thickness; // 0.0f - n
    };
}