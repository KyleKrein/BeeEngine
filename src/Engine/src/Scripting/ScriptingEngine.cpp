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
    };
    ScriptingEngineData ScriptingEngine::s_Data = {};
    std::unordered_map<String, MAssembly> ScriptingEngine::s_Assemblies = {};
    std::unordered_map<String, Ref<MClass>> ScriptingEngine::s_GameScripts = {};
    MClass* ScriptingEngine::s_EntityBaseClass = nullptr;
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
            ScriptGlue::Register();
            return s_Assemblies.at(name);
        }
        else
        {
            s_Assemblies[name] = {path};
            ScriptGlue::Register();
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
    bool ScriptingEngine::IsGameScript(const MClass& klass)
    {
        return klass.IsDerivedFrom(*s_EntityBaseClass);
    }
    class MAssembly &ScriptingEngine::LoadGameAssembly(const std::filesystem::path& path)
    {
        auto& assembly = LoadAssembly(path);
        for(auto& klass : assembly.GetClasses())
        {
            if(IsGameScript(*klass))
            {
                BeeCoreTrace("Found game script: {0}", klass->GetFullName());
                s_GameScripts[klass->GetFullName()] = klass;
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
        ScriptGlue::Register();
        return assembly;
    }

    MClass &ScriptingEngine::GetGameScript(const String &name)
    {
        return *s_GameScripts.at(name);
    }

    class MAssembly &ScriptingEngine::LoadCoreAssembly(const std::filesystem::path &path)
    {
        auto& assembly = LoadAssembly(path);
        for (auto& mClass : assembly.GetClasses())
        {
            if(mClass->GetName() == "Entity")
            {
                s_EntityBaseClass = mClass.get();
                break;
            }
        }

        for(auto& klass : assembly.GetClasses())
        {
            if(IsGameScript(*klass))
            {
                BeeCoreTrace("Found game script: {0}", klass->GetFullName());
                s_GameScripts[klass->GetFullName()] = klass;
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
        return assembly;
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
        return s_Assemblies.at("BeeEngine.Core");
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
}
