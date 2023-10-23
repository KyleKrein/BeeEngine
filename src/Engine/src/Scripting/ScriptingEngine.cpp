//
// Created by alexl on 29.07.2023.
//

#include "ScriptingEngine.h"
#include <mono/jit/jit.h>
#include "mono/metadata/assembly.h"
#include <mono/metadata/mono-config.h>
#include "Core/Logging/Log.h"
#include <filesystem>
#include "Core/ResourceManager.h"
#include "MClass.h"
#include "MField.h"
#include "MAssembly.h"
#include "ScriptGlue.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "MObject.h"
#include "GameScript.h"
#include "MUtils.h"
#include "AllocatorStatistics.h"
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/mono-gc.h>
#include "MTypes.h"
#include "FileSystem/File.h"

namespace BeeEngine
{
    typedef void(__stdcall *AddEntityScript)(uint64_t id, MonoObject* behaviour, MonoException** exception);
    typedef void(__stdcall *EntityWasRemoved)(uint64_t id, MonoException ** exception);
    struct ScriptingEngineData
    {
        MonoDomain* RootDomain = nullptr;
        MonoDomain* AppDomain = nullptr;

        AddEntityScript AddEntityScriptMethod = nullptr;
        EntityWasRemoved EntityWasRemovedMethod = nullptr;

        void(*EndSceneMethod)() = nullptr;

        MonoClass* ulongClass = nullptr;

        bool EnableDebugging = false;

        Scene* CurrentScene = nullptr;
        std::unordered_map<UUID, Ref<GameScript>> EntityObjects;
        std::unordered_map<MClass*, std::vector<GameScriptField>> EditableFieldsDefaults;

        std::unordered_map<String, MAssembly> Assemblies = {};
        std::unordered_map<String, Ref<MClass>> GameScripts = {};
        MClass* EntityBaseClass = nullptr;

        MField* AssetHandleField = nullptr;
        MClass* Texture2DClass = nullptr;
        MClass* FontClass = nullptr;

        MonoVTable* TimeVTable = nullptr;
        MonoClassField * DeltaTimeField = nullptr;
        MonoClassField* TotalTimeField = nullptr;

        Path GameAssemblyPath = "";
        Path CoreAssemblyPath = "";

        String Locale = Locale::GetSystemLocale();
    };
    ScriptingEngineData ScriptingEngine::s_Data = {};
    void ScriptingEngine::Init()
    {
        InitMono();
    }

    void ScriptingEngine::Shutdown()
    {
        s_Data.EntityObjects.clear();
        s_Data.GameScripts.clear();
        s_Data.EditableFieldsDefaults.clear();
        s_Data.Assemblies.clear();
        s_Data.CurrentScene = nullptr;
        s_Data.EntityBaseClass = nullptr;
        s_Data.AddEntityScriptMethod = nullptr;
        s_Data.EntityWasRemovedMethod = nullptr;
        s_Data.ulongClass = nullptr;

        MonoShutdown();
    }

    void ScriptingEngine::InitMono()
    {
        mono_set_assemblies_path("mono/lib");
        if(s_Data.EnableDebugging)
        {
            const char* argv[2] = {
                    "--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,suspend=n,loglevel=3,logfile=MonoDebugger.log",
                    "--soft-breakpoints"
            };
            mono_jit_parse_options(2, (char**)argv);
            mono_debug_init(MONO_DEBUG_FORMAT_MONO);
        }

        if constexpr (Application::GetOsPlatform() != OSPlatform::Windows)
        {
            //mono_set_crash_chaining(true);
            mono_config_parse("config");
        }

        s_Data.RootDomain = mono_jit_init("BeeEngineJITRuntime");
        BeeCoreAssert(s_Data.RootDomain, "Failed to initialize Mono JIT!");

        if(s_Data.EnableDebugging)
        {
            mono_debug_domain_create(s_Data.RootDomain);
        }
        mono_thread_set_main(mono_thread_current());

        CreateAppDomain();

        BeeCoreInfo("Mono JIT initialized successfully!");
    }

    void ScriptingEngine::CreateAppDomain()
    {
        s_Data.AppDomain = mono_domain_create_appdomain((char *)"BeeEngineAppDomain", nullptr);
        mono_domain_set(s_Data.AppDomain, true);
    }

    MAssembly& ScriptingEngine::LoadAssembly(const Path &path)
    {
        auto name = path.GetFileNameWithoutExtension().AsUTF8();

        s_Data.Assemblies[name] = {path, s_Data.EnableDebugging};
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
        BeeExpects(s_Data.EntityBaseClass != nullptr);
        return klass.IsDerivedFrom(*s_Data.EntityBaseClass);
    }
    void ScriptingEngine::LoadGameAssembly(const Path& path)
    {
        s_Data.GameAssemblyPath = path;
        if(!File::Exists(path))
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

    void ScriptingEngine::LoadCoreAssembly(const Path &path)
    {
        s_Data.CoreAssemblyPath = path;
        auto& assembly = LoadAssembly(path);
        for (auto& mClass : assembly.GetClasses())
        {
            if(mClass->GetName() == "Behaviour")
            {
                s_Data.EntityBaseClass = mClass.get();
                continue;
            }
            if(mClass->GetName() == "Asset")
            {
                s_Data.AssetHandleField = &mClass->GetField("m_Handle");
                continue;
            }
            if(mClass->GetName() == "Texture2D")
            {
                s_Data.Texture2DClass = mClass.get();
                continue;
            }
            if(mClass->GetName() == "Font")
            {
                s_Data.FontClass = mClass.get();
                continue;
            }
            if(mClass->GetName() == "Time")
            {
                s_Data.TimeVTable = mono_class_vtable(mono_domain_get(), mClass->m_MonoClass);
                s_Data.DeltaTimeField = mClass->GetField("m_DeltaTime");
                s_Data.TotalTimeField = mClass->GetField("m_TotalTime");
                continue;
            }

            if(s_Data.EntityBaseClass && s_Data.AssetHandleField &&
            s_Data.Texture2DClass && s_Data.FontClass &&
            s_Data.TotalTimeField && s_Data.DeltaTimeField && s_Data.TimeVTable)
            {
                break;
            }
        }
        MonoClass* lifeTimeManager = mono_class_from_name(assembly.m_MonoImage, "BeeEngine.Internal", "LifeTimeManager");
        MonoMethod* addEntityScriptMethod = mono_class_get_method_from_name(lifeTimeManager, "AddEntityScript", 2);
        MonoMethod* entityWasRemovedMethod = mono_class_get_method_from_name(lifeTimeManager, "EntityWasRemoved", 1);
        MonoMethod* endSceneMethod = mono_class_get_method_from_name(lifeTimeManager, "EndScene", 0);
        s_Data.AddEntityScriptMethod = (AddEntityScript)mono_method_get_unmanaged_thunk(addEntityScriptMethod);
        s_Data.EntityWasRemovedMethod = (EntityWasRemoved)mono_method_get_unmanaged_thunk(entityWasRemovedMethod);
        s_Data.EndSceneMethod = (void(*)())mono_method_get_unmanaged_thunk(endSceneMethod);
        s_Data.ulongClass = mono_class_from_name(mono_get_corlib(), "System", "UInt64");

    }

    void ScriptingEngine::OnRuntimeStart(Scene *scene)
    {
        s_Data.CurrentScene = scene;
    }

    void ScriptingEngine::OnRuntimeStop()
    {
        s_Data.CurrentScene = nullptr;
        s_Data.EndSceneMethod();
        s_Data.EntityObjects.clear();
    }

    void ScriptingEngine::OnEntityCreated(BeeEngine::Entity entity, MClass *pClass)
    {
        auto uuid = entity.GetUUID();
        if(s_Data.EntityObjects.contains(uuid))
        {
            return;
        }
        auto script = CreateRef<GameScript>(*pClass, entity, s_Data.Locale);
        s_Data.EntityObjects[uuid] = script;
        //
        {
            //MonoObject *entityID = mono_value_box(s_Data.AppDomain, s_Data.ulongClass, &ulongUUID);
            MonoException *exc = nullptr;
            MonoObject* instance = script->GetMObject().GetMonoObject();
            s_Data.AddEntityScriptMethod(uuid, instance, &exc);
            if(exc)
            {
                MonoString* msg = mono_object_to_string(reinterpret_cast<MonoObject*>(exc), nullptr);
                char* message = mono_string_to_utf8(msg);
                BeeCoreError("Exception while adding script for entity {} in C#: {}", uuid, message);
                mono_free(message);
            }
        }
    }
    void ScriptingEngine::OnEntityDestroyed(UUID uuid)
    {
        MonoException *exc = nullptr;
        bool contains = s_Data.EntityObjects.contains(uuid);
        if(contains)
        {
            s_Data.EntityObjects.at(uuid)->InvokeOnDestroy();
        }
        s_Data.EntityWasRemovedMethod(uuid, &exc);
        if(exc)
        {
            MonoString* msg = mono_object_to_string(reinterpret_cast<MonoObject*>(exc), nullptr);
            char* message = mono_string_to_utf8(msg);
            BeeCoreError("Exception while removing entity {} from C#: {}", uuid, message);
            mono_free(message);
        }
        s_Data.EntityObjects.erase(uuid);
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

        s_Data.EntityBaseClass = nullptr;
        s_Data.AssetHandleField = nullptr;
        s_Data.Texture2DClass = nullptr;
        s_Data.FontClass = nullptr;

        s_Data.TimeVTable = nullptr;
        s_Data.TotalTimeField = nullptr;
        s_Data.DeltaTimeField = nullptr;

        mono_domain_set(mono_get_root_domain(), false);
        mono_domain_unload(s_Data.AppDomain);

        CreateAppDomain();

        LoadCoreAssembly(s_Data.CoreAssemblyPath);
        LoadGameAssembly(s_Data.GameAssemblyPath);

        ScriptGlue::Register();
    }

    void ScriptingEngine::EnableDebugging()
    {
        BeeCoreAssert(s_Data.RootDomain == nullptr, "Cannot enable debugging after the runtime has been initialized!");
        s_Data.EnableDebugging = true;
    }

    void ScriptingEngine::UpdateAllocatorStatistics()
    {
        auto& stats = BeeEngine::Internal::AllocatorStatistics::GetStatistics();
        stats.totalAllocatedMemory -= stats.gcHeapSize;
        stats.gcUsedMemory.store(mono_gc_get_used_size());
        stats.gcGenerations.store(mono_gc_max_generation());
        stats.gcHeapSize.store(mono_gc_get_heap_size());
        stats.totalAllocatedMemory += stats.gcHeapSize;
    }

    void ScriptingEngine::SetAssetHandle(MObject& obj, MField& field, AssetHandle& handle, MType type)
    {
        BeeExpects(
                type == MType::Texture2D ||
                        type == MType::Font
        );
        if(type == MType::Texture2D)
        {
            MObject assetObj = s_Data.Texture2DClass->Instantiate();
            auto* assetMonoObj = assetObj.GetMonoObject();
            assetObj.SetFieldValue(*s_Data.AssetHandleField, &handle);
            obj.SetFieldValue(field, assetMonoObj);
            return;
        }
        if(type == MType::Font)
        {
            MObject assetObj = s_Data.FontClass->Instantiate();
            auto* assetMonoObj = assetObj.GetMonoObject();
            assetObj.SetFieldValue(*s_Data.AssetHandleField, &handle);
            obj.SetFieldValue(field, assetMonoObj);
            return;
        }
    }

    void ScriptingEngine::GetAssetHandle(void* monoObject, AssetHandle &handle)
    {
        MObject obj {(MonoObject*)monoObject};
        obj.GetFieldValue(*s_Data.AssetHandleField, &handle);
    }

    void ScriptingEngine::UpdateTime(double deltaTime, double totalTime)
    {
        if(!s_Data.DeltaTimeField)
            return;
        mono_field_static_set_value(s_Data.TimeVTable, s_Data.DeltaTimeField, &deltaTime);
        mono_field_static_set_value(s_Data.TimeVTable, s_Data.TotalTimeField, &totalTime);
    }
    void ScriptingEngine::SetLocale(const BeeEngine::String &locale)
    {
        s_Data.Locale = locale;
    }
    const String& ScriptingEngine::GetScriptingLocale()
    {
        return s_Data.Locale;
    }
}
