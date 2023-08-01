//
// Created by alexl on 29.07.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include <filesystem>
#include "MClass.h"
#include "Core/UUID.h"

namespace BeeEngine
{
    struct Entity;
    class ScriptingEngine
    {
    public:
        static void Init();
        static void Shutdown();

        static class MAssembly& LoadAssembly(const std::filesystem::path& path);

        static class MAssembly& LoadGameAssembly(const std::filesystem::path& path);
        static class MAssembly& LoadCoreAssembly(const std::filesystem::path& path);
        static class MAssembly& GetCoreAssembly();

        static void OnRuntimeStart(class Scene* scene);
        static void OnRuntimeStop();

        static MClass& GetGameScript(const String& name);
        static bool HasGameScript(const String& name)
        {
            return s_GameScripts.contains(name);
        }
        static const std::unordered_map<String, MClass>& GetGameScripts() { return s_GameScripts; }

        static void RegisterInternalCall(const std::string& name, void* method);

        static void OnEntityCreated(Entity entity, MClass *pClass);
        static void OnEntityDestroyed(BeeEngine::Entity entity);
        static void OnEntityUpdate(Entity entity);

        static Scene* GetSceneContext();
        static MClass& GetEntityClass()
        {
            BeeExpects(s_EntityBaseClass != nullptr);
            return *s_EntityBaseClass;
        }

        static class GameScript* GetEntityScriptInstance(BeeEngine::UUID uuid);

    private:
        static void InitMono();
        static bool IsGameScript(const MClass& klass);
        static struct ScriptingEngineData s_Data;

        static std::unordered_map<String, class MAssembly> s_Assemblies;
        static std::unordered_map<String, MClass> s_GameScripts;
        static MClass* s_EntityBaseClass;

        static void MonoShutdown();
    };
}
