//
// Created by alexl on 29.07.2023.
//

#include "ScriptingEngine.h"
#include <mono/jit/jit.h>
#include "mono/metadata/assembly.h"
#include "Core/Logging/Log.h"
#include <filesystem>
#include "Core/ResourceManager.h"
#include "MClass.h"
#include "MAssembly.h"

namespace BeeEngine
{
    struct ScriptingEngineData
    {
        MonoDomain* RootDomain = nullptr;
        MonoDomain* AppDomain = nullptr;
    };
    ScriptingEngineData ScriptingEngine::s_Data = {};
    std::unordered_map<String, MAssembly> ScriptingEngine::s_Assemblies = {};
    void ScriptingEngine::Init()
    {
        InitMono();
    }

    void ScriptingEngine::Shutdown()
    {
        MonoShutdown();
    }

    void ScriptingEngine::InitMono()
    {
        mono_set_assemblies_path("mono/lib");
        s_Data.RootDomain = mono_jit_init("BeeEngineJITRuntime");
        BeeCoreAssert(s_Data.RootDomain, "Failed to initialize Mono JIT!");
        s_Data.AppDomain = mono_domain_create_appdomain((char *)"BeeEngineAppDomain", nullptr);
        mono_domain_set(s_Data.AppDomain, true);

        BeeCoreInfo("Mono JIT initialized successfully!");
    }

    MAssembly& ScriptingEngine::LoadAssembly(const std::filesystem::path &path)
    {
        auto name = ResourceManager::GetNameFromFilePath(path.string());
        if(s_Assemblies.contains(name))
        {
            s_Assemblies.at(name).Reload();
            return s_Assemblies.at(name);
        }
        else
        {
            s_Assemblies.emplace(name, MAssembly(path));
            return s_Assemblies.at(name);
        }
    }

    void ScriptingEngine::MonoShutdown()
    {
#if 0 //TODO: Crashes. Fix later
        mono_domain_unload(s_Data.AppDomain);
        mono_jit_cleanup(s_Data.RootDomain);
#endif
        s_Data.RootDomain = nullptr;
        s_Data.AppDomain = nullptr;
        BeeCoreInfo("Mono JIT shutdown successfully!");
    }
}
