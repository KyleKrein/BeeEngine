//
// Created by alexl on 06.11.2023.
//

#pragma once
#include <cstdint>
namespace BeeEngine
{
    using float32_t = float;
    static_assert(sizeof(float32_t) == 4, "float32_t is not 4 bytes!");
    using float64_t = double;
    static_assert(sizeof(float64_t) == 8, "float64_t is not 8 bytes!");
    using bool8_t = bool;
    static_assert(sizeof(bool8_t) == 1, "bool8_t is not 1 byte!");
    using bool32_t = int32_t;
    static_assert(sizeof(bool32_t) == 4, "bool32_t is not 4 bytes!");
}