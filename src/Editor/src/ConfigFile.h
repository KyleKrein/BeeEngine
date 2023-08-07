//
// Created by alexl on 26.05.2023.
//

#pragma once
#include "BeeEngine.h"

namespace BeeEngine::Editor
{
    class ConfigFile
    {
    public:
        [[nodiscard("Config file is very important")]] static WindowProperties LoadWindowConfiguration() noexcept;
        static std::filesystem::path LoadCompilerConfiguration() noexcept;
    };
}
