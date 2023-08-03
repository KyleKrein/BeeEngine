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
#include "ScriptGlue.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "MObject.h"
#include "GameScript.h"
#include "MUtils.h"

namespace BeeEngine
{
    struct ScriptingEngineData
    {
        MonoDomain* RootDomain = nullptr;
        MonoDomain* AppDomain = nullptr;

        Scene* CurrentScene = nullptr;
        std::unordered_map<UUID, Ref<GameScript>> EntityObjects;
        std::unordered_map<MClass*, std::vector<GameScriptField>> EditableFieldsDefaults;

        std::unordered_map<String, MAssembly> Assemblies = {};
        std::unordered_map<String, Ref<MClass>> GameScripts = {};
        MClass* EntityBaseClass = nullptr;

        std::filesystem::path GameAssemblyPath = "";
        std::filesystem::path CoreAssemblyPath = "";
    };
    ScriptingEngineData ScriptingEngine::s_Data = {};
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
        CreateAppDomain();

        BeeCoreInfo("Mono JIT initialized successfully!");
    }

    void ScriptingEngine::CreateAppDomain()
    {
        s_Data.AppDomain = mono_domain_create_appdomain((char *)"BeeEngineAppDomain", nullptr);
        mono_domain_set(s_Data.AppDomain, true);
    }

    MAssembly& ScriptingEngine::LoadAssembly(const std::filesystem::path &path)
    {
        auto name = ResourceManager::GetNameFromFilePath(path.string());

        s_Data.Assemblies[name] = {path};
        return s_Data.Assemblies.at(name);
    }

    void ScriptingEngine::MonoShutdown()
    {
        mono_domain_set(mono_get_root_domain(), false);
        mono_domain_unload(s_Data.AppDomain);
        mono_jit_cleanup(s_Data.RootDomain);

        s_Data.RootDomain = nullptr;
        s_Data.AppDomain = nullptr;
        BeeCoreInfo("Mono JIT shutdown successfully!");
    }
    bool ScriptingEngine::IsGameScript(const MClass& klass)
    {
        return klass.IsDerivedFrom(*s_Data.EntityBaseClass);
    }
    void ScriptingEngine::LoadGameAssembly(const std::filesystem::path& path)
    {
        s_Data.GameAssemblyPath = path;
        if(!std::filesystem::exists(path))
        {
            BeeCoreWarn("Game assembly not found!");
            return;
        }
        auto& assembly = LoadAssembly(path);
        for(auto& klass : assembly.GetClasses())
        {
            if(IsGameScript(*klass))
            {
                BeeCoreTrace("Found game script: {0}", klass->GetFullName());
                s_Data.GameScripts[klass->GetFullName()] = klass;
                auto& fields = (s_Data.EditableFieldsDefaults[klass.get()] = {});
                MObject obj = klass->Instantiate();
                auto& klassFields = klass->GetFields();
                for(auto& fieldPair : klassFields)
                {
                    auto& field = fieldPair.second;
                    if(MUtils::IsSutableForEdit(field))
                    {
                        auto& scriptField = fields.emplace_back(field);
                        byte buffer[GameScriptField::MAX_FIELD_SIZE];
                        memset(buffer, 0, GameScriptField::MAX_FIELD_SIZE);
                        obj.GetFieldValue(field, buffer);
                        scriptField.SetData(buffer);
                    }
                }
            }
        }
    }

    MClass &ScriptingEngine::GetGameScript(const String &name)
    {
        return *s_Data.GameScripts.at(name);
    }

    void ScriptingEngine::LoadCoreAssembly(const std::filesystem::path &path)
    {
        s_Data.CoreAssemblyPath = path;
        auto& assembly = LoadAssembly(path);
        for (auto& mClass : assembly.GetClasses())
        {
            if(mClass->GetName() == "Entity")
            {
                s_Data.EntityBaseClass = mClass.get();
                break;
            }
        }
    }

    void ScriptingEngine::OnRuntimeStart(Scene *scene)
    {
        s_Data.CurrentScene = scene;
    }

    void ScriptingEngine::OnRuntimeStop()
    {
        s_Data.CurrentScene = nullptr;
        s_Data.EntityObjects.clear();
    }

    void ScriptingEngine::OnEntityCreated(BeeEngine::Entity entity, MClass *pClass)
    {
        auto uuid = entity.GetUUID();
        if(s_Data.EntityObjects.contains(uuid))
        {
            return;
        }
        auto script = CreateRef<GameScript>(*pClass, entity);
        s_Data.EntityObjects.emplace(entity.GetUUID(), script);
        script->InvokeOnCreate();
    }
    void ScriptingEngine::OnEntityDestroyed(BeeEngine::Entity entity)
    {
        s_Data.EntityObjects.erase(entity.GetUUID());
    }
    void ScriptingEngine::OnEntityUpdate(BeeEngine::Entity entity)
    {
        UUID uuid = entity.GetUUID();
        BeeCoreAssert(s_Data.EntityObjects.contains(uuid), "Entity does not have a script attached!");
        auto& script = s_Data.EntityObjects.at(uuid);
        script->InvokeOnUpdate();
    }

    Scene *ScriptingEngine::GetSceneContext()
    {
        return s_Data.CurrentScene;
    }

    class MAssembly &ScriptingEngine::GetCoreAssembly()
    {
        return s_Data.Assemblies.at("BeeEngine.Core");
    }

    class GameScript* ScriptingEngine::GetEntityScriptInstance(BeeEngine::UUID uuid)
    {
        if(!s_Data.EntityObjects.contains(uuid))
        {
            return nullptr;
        }
        return s_Data.EntityObjects.at(uuid).get();
    }

    std::vector<class GameScriptField> &ScriptingEngine::GetDefaultScriptFields(MClass *klass)
    {
        return s_Data.EditableFieldsDefaults.at(klass);
    }

    class MObject *ScriptingEngine::GetEntityScriptInstance(Entity entity)
    {
        if(!s_Data.EntityObjects.contains(entity.GetUUID()))
        {
            return nullptr;
        }
        return &s_Data.EntityObjects.at(entity.GetUUID())->GetMObject();
    }

    MClass &ScriptingEngine::GetEntityClass()
    {
        return *s_Data.EntityBaseClass;
    }

    bool ScriptingEngine::HasGameScript(const String &name)
    {
        return s_Data.GameScripts.contains(name);
    }

    const std::unordered_map<String, Ref<MClass>> &ScriptingEngine::GetGameScripts()
    {
        return s_Data.GameScripts;
    }

    void ScriptingEngine::ReloadAssemblies()
    {
        s_Data.GameScripts.clear();
        s_Data.EntityObjects.clear();
        s_Data.EditableFieldsDefaults.clear();
        s_Data.Assemblies.clear();

        mono_domain_set(mono_get_root_domain(), false);
        mono_domain_unload(s_Data.AppDomain);

        CreateAppDomain();

        LoadCoreAssembly(s_Data.CoreAssemblyPath);
        LoadGameAssembly(s_Data.GameAssemblyPath);

        ScriptGlue::Register();
    }
}
