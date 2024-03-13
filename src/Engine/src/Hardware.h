//
// Created by alexl on 23.11.2023.
//

#pragma once
#include "Core/Numbers.h"

namespace BeeEngine
{
    class Hardware
    {
    public:
        enum class SystemTheme
        {
            Unknown = 0,
            Light,
            Dark
        };
        static uint32_t GetNumberOfCores();
        static uint32_t GetSystemRAM();
        static SystemTheme GetSystemTheme();
        static bool HasRayTracingSupport();
    };
}