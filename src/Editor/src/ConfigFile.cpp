//
// Created by alexl on 26.05.2023.
//

#include "ConfigFile.h"

namespace BeeEngine::Editor
{

    WindowProperties ConfigFile::LoadWindowConfiguration() noexcept
    {
        return {1280, 720, "BeeEngine Editor", VSync::On};
    }

    std::filesystem::path ConfigFile::LoadCompilerConfiguration() noexcept
    {
        return std::filesystem::current_path();
    }
}
