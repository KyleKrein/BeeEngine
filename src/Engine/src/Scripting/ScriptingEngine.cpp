//
// Created by alexl on 29.07.2023.
//

#include "ScriptingEngine.h"
#include "Allocator/AllocatorStatistics.h"
#include "Core/Logging/Log.h"
#include "Core/Move.h"
#include "Core/Reflection.h"
#include "Core/ResourceManager.h"
#include "FileSystem/File.h"
#include "GameScript.h"
#include "MAssembly.h"
#include "MClass.h"
#include "MField.h"
#include "MObject.h"
#include "MTypes.h"
#include "MUtils.h"
#include "NativeToManaged.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "ScriptGlue.h"
#include <filesystem>

namespace BeeEngine
{
    // typedef void(__stdcall *AddEntityScript)(uint64_t id, MonoObject* behaviour, MonoException** exception);
    // typedef void(__stdcall *EntityWasRemoved)(uint64_t id, MonoException ** exception);
    struct ManagedHandles
    {
        REFLECT()
        MMethod* AddEntityScriptMethod = nullptr;
        MMethod* EntityWasRemovedMethod = nullptr;

        MMethod* EndSceneMethod = nullptr;

        MClass* EntityBaseClass = nullptr;

        MField* AssetHandleField = nullptr;
        MClass* Texture2DClass = nullptr;
        MClass* FontClass = nullptr;
        MClass* PrefabClass = nullptr;
        MClass* SceneClass = nullptr;

        MClass* InternalCallsClass = nullptr;

        MField* DeltaTimeField = nullptr;
        MField* TotalTimeField = nullptr;
    };

    REFLECT_STRUCT_BEGIN(ManagedHandles)
    REFLECT_STRUCT_MEMBER(AddEntityScriptMethod)
    REFLECT_STRUCT_MEMBER(EntityWasRemovedMethod)
    REFLECT_STRUCT_MEMBER(EndSceneMethod)
    REFLECT_STRUCT_MEMBER(EntityBaseClass)
    REFLECT_STRUCT_MEMBER(AssetHandleField)
    REFLECT_STRUCT_MEMBER(Texture2DClass)
    REFLECT_STRUCT_MEMBER(FontClass)
    REFLECT_STRUCT_MEMBER(PrefabClass)
    REFLECT_STRUCT_MEMBER(SceneClass)
    REFLECT_STRUCT_MEMBER(InternalCallsClass)
    REFLECT_STRUCT_MEMBER(DeltaTimeField)
    REFLECT_STRUCT_MEMBER(TotalTimeField)
    REFLECT_STRUCT_END()

    struct ScriptingEngineData
    {
        bool EnableDebugging = false;

        Scene* CurrentScene = nullptr;
        std::unordered_map<UUID, Ref<GameScript>> EntityObjects;
        std::unordered_map<MClass*, std::vector<GameScriptField>> EditableFieldsDefaults;

        std::unordered_map<String, MAssembly> Assemblies = {};
        std::unordered_map<String, Ref<MClass>> GameScripts = {};

        ManagedAssemblyContextID AppDomain = 0;

        Path GameAssemblyPath = "";
        Path CoreAssemblyPath = "";
        Path GameAssemblyDebugSymbolsPath = "";
        Path CoreAssemblyDebugSymbolsPath = "";

        Locale::Domain* LocaleDomain = nullptr;

        int MouseX = 0;
        int MouseY = 0;
        glm::vec2 ViewportSize = {-1, -1};

        ManagedHandles Handles = {};

        std::function<void(AssetHandle)> OnSceneChangeCallback = nullptr;
    };
    ScriptingEngineData ScriptingEngine::s_Data = {};

    void ScriptingEngine::Init(std::function<void(AssetHandle)> onSceneChangeCallback)
    {
        // InitMono();
        s_Data.OnSceneChangeCallback = BeeMove(onSceneChangeCallback);
        InitDotNetHost();
    }

    void ScriptingEngine::Shutdown()
    {
        s_Data.EntityObjects.clear();
        s_Data.GameScripts.clear();
        s_Data.EditableFieldsDefaults.clear();
        s_Data.Assemblies.clear();
        s_Data.CurrentScene = nullptr;
        s_Data.Handles = {};
        // s_Data.AddEntityScriptMethod = nullptr;
        // s_Data.EntityWasRemovedMethod = nullptr;

        MonoShutdown();
    }

    bool ScriptingEngine::IsInitialized()
    {
        return s_Data.AppDomain != 0;
    }

    void ScriptingEngine::InitMono()
    {
        /*mono_set_assemblies_path("mono/lib");
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

        BeeCoreInfo("Mono JIT initialized successfully!");*/
    }

    void ScriptingEngine::CreateAppDomain()
    {
        s_Data.AppDomain = NativeToManaged::CreateContext("BeeEngine", true);
        BeeEnsures(s_Data.AppDomain);
    }

    MAssembly& ScriptingEngine::LoadAssembly(const Path& path, const Path& debugSymbolsPath)
    {
        auto name = path.GetFileNameWithoutExtension().AsUTF8();

        s_Data.Assemblies[name] = {
            s_Data.AppDomain, path.IsAbsolute() ? path : path.GetAbsolutePath(), debugSymbolsPath};
        return s_Data.Assemblies.at(name);
    }

    void ScriptingEngine::MonoShutdown()
    {
        /*
        MUtils::RegisterThread();
        mono_domain_set(mono_get_root_domain(), false);
        mono_domain_unload(s_Data.AppDomain);
        mono_jit_cleanup(s_Data.RootDomain);

        s_Data.RootDomain = nullptr;
        s_Data.AppDomain = nullptr;
        BeeCoreInfo("Mono JIT shutdown successfully!");
        */
    }
    bool ScriptingEngine::IsGameScript(const MClass& klass)
    {
        BeeExpects(s_Data.Handles.EntityBaseClass != nullptr);
        return klass.IsDerivedFrom(*s_Data.Handles.EntityBaseClass);
    }
    void ScriptingEngine::LoadGameAssembly(const Path& path)
    {
        LoadGameAssembly(path, {});
    }
    void ScriptingEngine::LoadGameAssembly(const Path& path, const Path& debugSymbolsPath)
    {
        s_Data.GameAssemblyPath = path;
        s_Data.GameAssemblyDebugSymbolsPath = debugSymbolsPath;
        if (!File::Exists(path))
        {
            BeeCoreWarn("Game assembly not found!");
            return;
        }
        auto& assembly = LoadAssembly(path, debugSymbolsPath);
        for (auto& klass : assembly.GetClasses())
        {
            if (IsGameScript(*klass))
            {
                BeeCoreTrace("Found game script: {0}", klass->GetFullName());
                s_Data.GameScripts[klass->GetFullName()] = klass;
                auto& fields = (s_Data.EditableFieldsDefaults[klass.get()] = {});
                MObject obj = klass->Instantiate();
                auto& klassFields = klass->GetFields();
                for (auto& fieldPair : klassFields)
                {
                    auto& field = fieldPair.second;
                    if (MUtils::IsSutableForEdit(field))
                    {
                        auto& scriptField = fields.emplace_back(field);
                        auto data = obj.GetFieldValue(field);
                        scriptField.SetData(data);
                    }
                }
            }
        }
    }

    MClass& ScriptingEngine::GetGameScript(const String& name)
    {
        return *s_Data.GameScripts.at(name);
    }

    bool ScriptingEngine::AreAllManagedHandlesLoaded()
    {
        const auto& typeDescriptor = Reflection::TypeResolver::Get<ManagedHandles>();
        for (const auto& field : typeDescriptor.Members)
        {
            if (field.Get<void*>(s_Data.Handles) == nullptr)
            {
                return false;
            }
        }
        return true;
    }
    void ScriptingEngine::LoadCoreAssembly(const Path& path)
    {
        LoadCoreAssembly(path, {});
    }
    void ScriptingEngine::LoadCoreAssembly(const Path& path, const Path& debugSymbolsPath)
    {
        s_Data.CoreAssemblyPath = path;
        s_Data.CoreAssemblyDebugSymbolsPath = debugSymbolsPath;
        auto& assembly = LoadAssembly(path, debugSymbolsPath);
        for (auto& mClass : assembly.GetClasses())
        {
            if (mClass->GetName() == "Behaviour")
            {
                s_Data.Handles.EntityBaseClass = mClass.get();
                continue;
            }
            if (mClass->GetName() == "Asset")
            {
                s_Data.Handles.AssetHandleField = &mClass->GetField("m_Handle");
                continue;
            }
            if (mClass->GetName() == "Texture2D")
            {
                s_Data.Handles.Texture2DClass = mClass.get();
                continue;
            }
            if (mClass->GetName() == "Font")
            {
                s_Data.Handles.FontClass = mClass.get();
                continue;
            }
            if (mClass->GetName() == "Prefab")
            {
                s_Data.Handles.PrefabClass = mClass.get();
                continue;
            }
            if (mClass->GetName() == "Scene")
            {
                s_Data.Handles.SceneClass = mClass.get();
                continue;
            }
            if (mClass->GetName() == "Time")
            {
                // s_Data.TimeVTable = mono_class_vtable(mono_domain_get(), mClass->m_MonoClass);
                s_Data.Handles.DeltaTimeField = &mClass->GetField("m_DeltaTime");
                s_Data.Handles.TotalTimeField = &mClass->GetField("m_TotalTime");
                continue;
            }
            if (mClass->GetName() == "LifeTimeManager")
            {
                static constexpr auto flags = static_cast<ManagedBindingFlags>(
                    ManagedBindingFlags_Public | ManagedBindingFlags_Static | ManagedBindingFlags_NonPublic);
                s_Data.Handles.AddEntityScriptMethod = &mClass->GetMethod("AddEntityScript", flags);
                s_Data.Handles.EntityWasRemovedMethod = &mClass->GetMethod("EntityWasRemoved", flags);
                s_Data.Handles.EndSceneMethod = &mClass->GetMethod("EndScene", flags);
            }
            if (mClass->GetName() == "InternalCalls")
            {
                s_Data.Handles.InternalCallsClass = mClass.get();
            }

            if (AreAllManagedHandlesLoaded())
            {
                break;
            }
        }

        BeeEnsures(AreAllManagedHandlesLoaded());
    }

    void ScriptingEngine::OnRuntimeStart(Scene* scene)
    {
        s_Data.CurrentScene = scene;
    }

    void ScriptingEngine::OnRuntimeStop()
    {
        s_Data.CurrentScene = nullptr;
        s_Data.Handles.EndSceneMethod->InvokeStatic(nullptr);
        s_Data.EntityObjects.clear();
    }

    void ScriptingEngine::OnEntityCreated(BeeEngine::Entity entity, MClass* pClass)
    {
        auto uuid = entity.GetUUID();
        if (s_Data.EntityObjects.contains(uuid))
        {
            return;
        }
        auto script = CreateRef<GameScript>(*pClass, entity, GetScriptingLocale());
        s_Data.EntityObjects[uuid] = script;
        {
            auto instance = script->GetMObject().GetHandle();
            void* params[] = {&uuid, instance};
            BeeCoreTrace("Params: {} {}", (uintptr_t)params[0], (uintptr_t)params[1]);
            s_Data.Handles.AddEntityScriptMethod->InvokeStatic(params);
        }
    }
    void ScriptingEngine::OnEntityDestroyed(UUID uuid)
    {
        // MonoException *exc = nullptr;
        bool contains = s_Data.EntityObjects.contains(uuid);
        if (contains)
        {
            s_Data.EntityObjects.at(uuid)->InvokeOnDestroy();
        }
        void* params[] = {&uuid};
        s_Data.Handles.EntityWasRemovedMethod->InvokeStatic(params);
        // s_Data.EntityWasRemovedMethod(uuid, &exc);
        // if(exc)
        {
            // MonoString* msg = mono_object_to_string(reinterpret_cast<MonoObject*>(exc), nullptr);
            // char* message = mono_string_to_utf8(msg);
            // BeeCoreError("Exception while removing entity {} from C#: {}", uuid, message);
            // mono_free(message);
        }
        s_Data.EntityObjects.erase(uuid);
    }
    void ScriptingEngine::OnEntityUpdate(BeeEngine::Entity entity)
    {
        UUID uuid = entity.GetUUID();
        if (!s_Data.EntityObjects.contains(uuid))
        {
            return;
        }
        // BeeCoreAssert(s_Data.EntityObjects.contains(uuid), "Entity does not have a script attached!");
        auto& script = s_Data.EntityObjects.at(uuid);
        script->InvokeOnUpdate();
    }

    Scene* ScriptingEngine::GetSceneContext()
    {
        return s_Data.CurrentScene;
    }

    class MAssembly& ScriptingEngine::GetCoreAssembly()
    {
        return s_Data.Assemblies.at("BeeEngine.Core");
    }

    class GameScript* ScriptingEngine::GetEntityScriptInstance(BeeEngine::UUID uuid)
    {
        if (!s_Data.EntityObjects.contains(uuid))
        {
            return nullptr;
        }
        return s_Data.EntityObjects.at(uuid).get();
    }

    std::vector<class GameScriptField>& ScriptingEngine::GetDefaultScriptFields(MClass* klass)
    {
        return s_Data.EditableFieldsDefaults.at(klass);
    }

    class MObject* ScriptingEngine::GetEntityScriptInstance(Entity entity)
    {
        if (!s_Data.EntityObjects.contains(entity.GetUUID()))
        {
            return nullptr;
        }
        return &s_Data.EntityObjects.at(entity.GetUUID())->GetMObject();
    }

    MClass& ScriptingEngine::GetEntityClass()
    {
        return *s_Data.Handles.EntityBaseClass;
    }

    bool ScriptingEngine::HasGameScript(const String& name)
    {
        return s_Data.GameScripts.contains(name);
    }

    const std::unordered_map<String, Ref<MClass>>& ScriptingEngine::GetGameScripts()
    {
        return s_Data.GameScripts;
    }

    void ScriptingEngine::RegisterNativeFunction(const String& name, void* function)
    {
        MMethod& assignFunction = s_Data.Handles.InternalCallsClass->GetMethod(
            "AssignFunction", (ManagedBindingFlags)(ManagedBindingFlags_NonPublic | ManagedBindingFlags_Static));
        ;
        GCHandle str = NativeToManaged::StringCreateManaged(name);
        void* params[] = {&str, &function};
        assignFunction.InvokeStatic(params);
        NativeToManaged::ObjectFreeGCHandle(str);
    }

    void ScriptingEngine::ReloadAssemblies()
    {
        UnloadAppContext();

        CreateAppDomain();
        LoadCoreAssembly(s_Data.CoreAssemblyPath, s_Data.CoreAssemblyDebugSymbolsPath);
        LoadGameAssembly(s_Data.GameAssemblyPath, s_Data.GameAssemblyDebugSymbolsPath);

        ScriptGlue::Register();
    }

    void ScriptingEngine::EnableDebugging()
    {
        // BeeCoreAssert(s_Data.RootDomain == nullptr, "Cannot enable debugging after the runtime has been
        // initialized!");
        s_Data.EnableDebugging = true;
    }

    void ScriptingEngine::UpdateAllocatorStatistics()
    {
        auto& stats = BeeEngine::Internal::AllocatorStatistics::GetStatistics();
        stats.totalAllocatedMemory -= stats.gcHeapSize;
        // stats.gcUsedMemory.store(mono_gc_get_used_size());
        // stats.gcGenerations.store(mono_gc_max_generation());
        // stats.gcHeapSize.store(mono_gc_get_heap_size());
        stats.totalAllocatedMemory += stats.gcHeapSize;
    }

    void ScriptingEngine::SetAssetHandle(MObject& obj, MField& field, AssetHandle& handle, MType type)
    {
        BeeExpects(type == MType::Texture2D || type == MType::Font || type == MType::Prefab || type == MType::Scene);
        if (type == MType::Texture2D)
        {
            MObject assetObj = s_Data.Handles.Texture2DClass->Instantiate();
            assetObj.SetFieldValue(*s_Data.Handles.AssetHandleField, &handle);
            obj.SetFieldValue(field, assetObj.GetHandle());
            return;
        }
        if (type == MType::Font)
        {
            MObject assetObj = s_Data.Handles.FontClass->Instantiate();
            assetObj.SetFieldValue(*s_Data.Handles.AssetHandleField, &handle);
            obj.SetFieldValue(field, assetObj.GetHandle());
            return;
        }
        if (type == MType::Prefab)
        {
            MObject assetObj = s_Data.Handles.PrefabClass->Instantiate();
            assetObj.SetFieldValue(*s_Data.Handles.AssetHandleField, &handle);
            obj.SetFieldValue(field, assetObj.GetHandle());
            return;
        }
        if (type == MType::Scene)
        {
            MObject assetObj = s_Data.Handles.SceneClass->Instantiate();
            assetObj.SetFieldValue(*s_Data.Handles.AssetHandleField, &handle);
            obj.SetFieldValue(field, assetObj.GetHandle());
            return;
        }
    }

    void ScriptingEngine::GetAssetHandle(void* gchandle, AssetHandle& handle)
    {
        auto& field = *s_Data.Handles.AssetHandleField;
        auto& mclass = field.GetClass();
        handle = *static_cast<AssetHandle*>(NativeToManaged::FieldGetData(
            mclass.GetContextID(), mclass.GetAssemblyID(), mclass.GetClassID(), field.GetFieldID(), gchandle));
        // obj.GetFieldValue(*s_Data.AssetHandleField, &handle);
    }

    void ScriptingEngine::UpdateTime(Time::secondsD deltaTime, Time::secondsD totalTime)
    {
        if (!s_Data.Handles.DeltaTimeField)
            return;
        double deltaTimeDouble = deltaTime.count();
        double totalTimeDouble = totalTime.count();
        auto& mclass = s_Data.Handles.DeltaTimeField->GetClass();
        NativeToManaged::FieldSetData(mclass.GetContextID(),
                                      mclass.GetAssemblyID(),
                                      mclass.GetClassID(),
                                      s_Data.Handles.DeltaTimeField->GetFieldID(),
                                      nullptr,
                                      &deltaTimeDouble);
        NativeToManaged::FieldSetData(mclass.GetContextID(),
                                      mclass.GetAssemblyID(),
                                      mclass.GetClassID(),
                                      s_Data.Handles.TotalTimeField->GetFieldID(),
                                      nullptr,
                                      &totalTimeDouble);
        // mono_field_static_set_value(s_Data.TimeVTable, s_Data.DeltaTimeField, &deltaTimeDouble);
        // mono_field_static_set_value(s_Data.TimeVTable, s_Data.TotalTimeField, &totalTimeDouble);
    }
    const String& ScriptingEngine::GetScriptingLocale()
    {
        return s_Data.LocaleDomain->GetLocale().GetLanguageString();
    }

    void ScriptingEngine::SetMousePosition(int x, int y)
    {
        s_Data.MouseX = x;
        s_Data.MouseY = y;
    }

    glm::vec2 ScriptingEngine::GetMousePosition()
    {
        return {s_Data.MouseX, s_Data.MouseY};
    }

    glm::vec2 ScriptingEngine::GetViewportSize()
    {
        if (s_Data.ViewportSize.x == -1 || s_Data.ViewportSize.y == -1)
        {
            auto& app = Application::GetInstance();
            return {app.GetWidth(), app.GetHeight()};
        }
        return s_Data.ViewportSize;
    }

    void ScriptingEngine::SetViewportSize(float width, float height)
    {
        s_Data.ViewportSize = {width, height};
    }

    void ScriptingEngine::UnloadAppContext()
    {
        if (s_Data.AppDomain == 0)
        {
            return;
        }
        s_Data.EditableFieldsDefaults.clear();
        s_Data.EntityObjects.clear();
        s_Data.GameScripts.clear();
        s_Data.Assemblies.clear();

        s_Data.Handles = {};

        NativeToManaged::UnloadContext(s_Data.AppDomain);
        s_Data.AppDomain = 0;
    }

    void ScriptingEngine::InitDotNetHost()
    {
        Path rootPath = std::filesystem::current_path() / "libs" / "BeeEngine.NativeBridge.dll";
        NativeToManaged::Init(rootPath);
        NativeToManaged::SetupLogger();
        CreateAppDomain();
    }

    void ScriptingEngine::SetLocaleDomain(Locale::Domain& domain)
    {
        s_Data.LocaleDomain = &domain;
    }

    Locale::Domain& ScriptingEngine::GetLocaleDomain()
    {
        return *s_Data.LocaleDomain;
    }

    void ScriptingEngine::RequestSceneChange(AssetHandle sceneHandle)
    {
        s_Data.OnSceneChangeCallback(sceneHandle);
    }
} // namespace BeeEngine