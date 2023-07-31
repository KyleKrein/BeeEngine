//
// Created by alexl on 30.07.2023.
//

#include "ScriptGlue.h"
#include "ScriptingEngine.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Core/Input.h"
#include "MAssembly.h"
#include "Core/Logging/GameLogger.h"


#define BEE_INTERNAL_CALL(name) ScriptingEngine::RegisterInternalCall("BeeEngine.Internal.InternalCalls::" #name, (void*)&name)

namespace BeeEngine
{

    std::unordered_map<MonoType*, std::function<void*(Entity)>> s_CreateComponentFunctions;
    std::unordered_map<MonoType*, std::function<void*(Entity)>> s_GetComponentFunctions;
    std::unordered_map<MonoType*, std::function<bool(Entity)>> s_HasComponentFunctions;
    std::unordered_map<MonoType*, std::function<void(Entity)>> s_RemoveComponentFunctions;

    template<typename ...Component>
    void ScriptGlue::RegisterComponent()
    {
        ([](){
            auto typeName = TypeName<Component>();
            size_t pos = typeName.find_last_of(':');
            std::string managedTypeName = fmt::format("BeeEngine.{}", typeName.substr(pos + 1));
            MonoType* managedType = mono_reflection_type_from_name((char*)managedTypeName.c_str(), ScriptingEngine::GetCoreAssembly().m_MonoImage);
            if(!managedType)
            {
                BeeCoreError("Could not find C# component type {}", managedTypeName);
                return;
            }                                            \
            s_CreateComponentFunctions[managedType] = [](Entity entity) {return &entity.AddComponent<Component>();};
            s_GetComponentFunctions[managedType] = [](Entity entity) {return &entity.GetComponent<Component>();};
            s_HasComponentFunctions[managedType] = [](Entity entity) {return entity.HasComponent<Component>();};
            s_RemoveComponentFunctions[managedType] = [](Entity entity) {entity.RemoveComponent<Component>();};

        }(), ...);
    }

    template<typename ...Component>
    void ScriptGlue::RegisterComponent(ComponentGroup<Component...>)
    {
        RegisterComponent<Component...>();
    }
    void ScriptGlue::Register()
    {
        if(!s_CreateComponentFunctions.empty())
        {
            s_CreateComponentFunctions.clear();
            s_GetComponentFunctions.clear();
            s_HasComponentFunctions.clear();
            s_RemoveComponentFunctions.clear();
        }
        {
            RegisterComponent(AllComponents{});
        }
        {//Internal Calls
            BEE_INTERNAL_CALL(Log_Warn);
            BEE_INTERNAL_CALL(Log_Info);
            BEE_INTERNAL_CALL(Log_Error);
            BEE_INTERNAL_CALL(Log_Trace);

            BEE_INTERNAL_CALL(Entity_GetComponent);
            BEE_INTERNAL_CALL(Entity_CreateComponent);
            BEE_INTERNAL_CALL(Entity_HasComponent);
            BEE_INTERNAL_CALL(Entity_RemoveComponent);

            BEE_INTERNAL_CALL(Entity_GetTransformComponent);
            BEE_INTERNAL_CALL(Entity_GetTranslation);
            BEE_INTERNAL_CALL(Entity_SetTranslation);

            BEE_INTERNAL_CALL(Input_IsKeyDown);
            BEE_INTERNAL_CALL(Input_IsMouseButtonDown);
        }
    }

    void ScriptGlue::Log_Warn(MonoString *message)
    {
        char* msg = mono_string_to_utf8(message);
        BeeWarn(msg);
        mono_free(msg);
    }

    void ScriptGlue::Log_Info(MonoString *message)
    {
        char* msg = mono_string_to_utf8(message);
        BeeInfo(msg);
        mono_free(msg);
    }

    void ScriptGlue::Log_Error(MonoString *message)
    {
        char* msg = mono_string_to_utf8(message);
        BeeError(msg);
        mono_free(msg);
    }

    void ScriptGlue::Log_Trace(MonoString *message)
    {
        char* msg = mono_string_to_utf8(message);
        BeeTrace(msg);
        mono_free(msg);
    }

    void ScriptGlue::Entity_GetTranslation(UUID id, glm::vec3 *outTranslation)
    {
        auto* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(id);
        *outTranslation = entity.GetComponent<TransformComponent>().Translation;
    }

    void ScriptGlue::Entity_SetTranslation(UUID id, glm::vec3 *inTranslation)
    {
        auto* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(id);
        entity.GetComponent<TransformComponent>().Translation = *inTranslation;
    }

    bool ScriptGlue::Input_IsKeyDown(Key key)
    {
        return Input::KeyPressed(key);
    }

    bool ScriptGlue::Input_IsMouseButtonDown(MouseButton button)
    {
        return Input::MouseKeyPressed(button);
    }

    void *ScriptGlue::Entity_GetTransformComponent(UUID id)
    {
        auto* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(id);
        return &entity.GetComponent<TransformComponent>();
    }

    void *ScriptGlue::Entity_CreateComponent(UUID id, MonoReflectionType *reflectionType)
    {
        auto entity = GetEntity(id);
        BeeExpects(entity);
        BeeExpects(reflectionType != nullptr);
        MonoType* managedType = mono_reflection_type_get_type(reflectionType);
#if defined(BEE_ENABLE_CHECKS)
        if(s_CreateComponentFunctions.contains(managedType))
        {
#endif
                return s_CreateComponentFunctions.at(managedType)(entity);
#if defined(BEE_ENABLE_CHECKS)
        }
        else
        {
            char* typeName = mono_type_get_name(managedType);
            BeeError("Could not find C# component type {}", typeName);
            mono_free(typeName);
            return nullptr;
        }
#endif
    }

    class Entity ScriptGlue::GetEntity(UUID id)
    {
        auto* scene = ScriptingEngine::GetSceneContext();
        return scene->GetEntityByUUID(id);
    }

    bool ScriptGlue::Entity_HasComponent(UUID id, MonoReflectionType *reflectionType)
    {
        auto entity = GetEntity(id);
        BeeExpects(entity);
        BeeExpects(reflectionType != nullptr);
        MonoType* managedType = mono_reflection_type_get_type(reflectionType);
        if(s_HasComponentFunctions.contains(managedType))
        {
            return s_HasComponentFunctions.at(managedType)(entity);
        }
        else
        {
            char* typeName = mono_type_get_name(managedType);
            BeeError("Could not find C# component type {}", typeName);
            mono_free(typeName);
            return false;
        }
    }

    void ScriptGlue::Entity_RemoveComponent(UUID id, MonoReflectionType *reflectionType)
    {
        auto entity = GetEntity(id);
        BeeExpects(entity);
        BeeExpects(reflectionType != nullptr);
        MonoType* managedType = mono_reflection_type_get_type(reflectionType);
#if defined(BEE_ENABLE_CHECKS)
        if(s_RemoveComponentFunctions.contains(managedType))
        {
#endif
                s_RemoveComponentFunctions.at(managedType)(entity);
#if defined(BEE_ENABLE_CHECKS)
                return;
        }
        else
        {
            char* typeName = mono_type_get_name(managedType);
            BeeError("Could not find C# component type {}", typeName);
            mono_free(typeName);
        }
#endif
    }

    void *ScriptGlue::Entity_GetComponent(UUID id, MonoReflectionType *reflectionType)
    {
        auto entity = GetEntity(id);
        BeeExpects(entity);
        BeeExpects(reflectionType != nullptr);
        MonoType* managedType = mono_reflection_type_get_type(reflectionType);
#if defined(BEE_ENABLE_CHECKS)
        if(s_GetComponentFunctions.contains(managedType))
        {
#endif
                return s_GetComponentFunctions.at(managedType)(entity);
#if defined(BEE_ENABLE_CHECKS)
        }
        else
        {
            char* typeName = mono_type_get_name(managedType);
            BeeError("Could not find C# component type {}", typeName);
            mono_free(typeName);
            return nullptr;
        }
#endif
    }
}
