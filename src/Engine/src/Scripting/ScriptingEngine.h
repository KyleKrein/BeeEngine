//
// Created by alexl on 29.07.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include <filesystem>
#include "Core/UUID.h"

namespace BeeEngine
{
    struct Entity;
    class MClass;
    class ScriptingEngine
    {
    public:
        static void Init();
        static void Shutdown();

        static void LoadGameAssembly(const std::filesystem::path& path);

        static void LoadCoreAssembly(const std::filesystem::path& path);
        static void ReloadAssemblies();
        static class MAssembly& GetCoreAssembly();
        static void OnRuntimeStart(class Scene* scene);

        static void OnRuntimeStop();
        static MClass& GetGameScript(const String& name);

        static bool HasGameScript(const String& name);
        static const std::unordered_map<String, Ref<MClass>>& GetGameScripts();
        static void RegisterInternalCall(const std::string& name, void* method);

        static void OnEntityCreated(Entity entity, MClass *pClass);

        static void OnEntityDestroyed(BeeEngine::Entity entity);
        static void OnEntityUpdate(Entity entity);
        static class MObject* GetEntityScriptInstance(Entity entity);

        static Scene* GetSceneContext();

        static MClass& GetEntityClass();
        static class GameScript* GetEntityScriptInstance(BeeEngine::UUID uuid);

        static std::vector<class GameScriptField>& GetDefaultScriptFields(MClass* klass);
    private:

        static class MAssembly& LoadAssembly(const std::filesystem::path& path);
        static void InitMono();
        static bool IsGameScript(const MClass& klass);
        static struct ScriptingEngineData s_Data;

        static void MonoShutdown();

        static void CreateAppDomain();
    };
}
