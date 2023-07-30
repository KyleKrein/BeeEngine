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
        return R"("C:\Users\alexl\Downloads\llvm-mingw-20230517-ucrt-x86_64\llvm-mingw-20230517-ucrt-x86_64\bin\clang++.exe" )";
    }
}
