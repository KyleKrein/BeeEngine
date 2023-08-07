//
// Created by alexl on 26.07.2023.
//

#pragma once
#include <filesystem>
#include "Utils/DynamicLibrary.h"
#include "Core/TypeDefines.h"
namespace BeeEngine::Editor
{
    class GameBuilder
    {
    public:
        GameBuilder(const std::filesystem::path& path, const std::filesystem::path& compilerPath);
        void Load();
        void Unload();
        void Build();
        bool UpdateAndCompile();
        ~GameBuilder();
    private:
        void RCCPPLoad(const std::filesystem::path& compilerPath);

        Ref<DynamicLibrary> m_GameLibrary = nullptr;
        std::filesystem::path m_WorkingDirectory;
        std::vector<std::string> m_TemporaryNames;
        std::string m_NativeGameLibraryName;
        constexpr static const char* s_GameLibraryName = "GameLibrary";
    };
}
