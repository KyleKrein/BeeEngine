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
        /// @brief Returns the number of CPU cores from currently used system.
        /// @return Number of CPU cores
        static uint32_t GetNumberOfCores();
        /// @brief Returns the amount of RAM in bytes from currently used system.
        /// @return Amount of RAM in bytes
        static uint32_t GetSystemRAM();
        /// @brief Returns the amount of VRAM in bytes from currently used GPU.
        /// @return Amount of VRAM in bytes
        static uint64_t GetGPUVRAM();
        static SystemTheme GetSystemTheme();
        static bool HasRayTracingSupport();
    };
} // namespace BeeEngine