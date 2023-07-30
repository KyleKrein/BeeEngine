//
// Created by alexl on 29.07.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include <filesystem>

namespace BeeEngine
{
    class ScriptingEngine
    {
    public:
        static void Init();
        static void Shutdown();

        static class MAssembly& LoadAssembly(const std::filesystem::path& path);
    private:
        static void InitMono();
        static struct ScriptingEngineData s_Data;

        static std::unordered_map<String, class MAssembly> s_Assemblies;

        static void MonoShutdown();
    };
}
