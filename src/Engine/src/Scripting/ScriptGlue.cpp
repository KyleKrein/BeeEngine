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
    void ScriptGlue::RegisterComponent(TypeSequence<Component...>)
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
            BEE_INTERNAL_CALL(Entity_FindEntityByName);
            BEE_INTERNAL_CALL(Entity_GetScriptInstance);
            BEE_INTERNAL_CALL(Entity_Destroy);

            BEE_INTERNAL_CALL(Entity_GetTransformComponent);
            BEE_INTERNAL_CALL(Entity_GetTranslation);
            BEE_INTERNAL_CALL(Entity_SetTranslation);

            BEE_INTERNAL_CALL(TextRendererComponent_GetText);
            BEE_INTERNAL_CALL(TextRendererComponent_SetText);

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

    void ScriptGlue::Entity_GetTranslation(uint64_t id, glm::vec3 *outTranslation)
    {
        auto* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(id);
        *outTranslation = entity.GetComponent<TransformComponent>().Translation;
    }

    void ScriptGlue::Entity_SetTranslation(uint64_t id, glm::vec3 *inTranslation)
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

    void *ScriptGlue::Entity_GetTransformComponent(uint64_t id)
    {
        auto* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(id);
        return &entity.GetComponent<TransformComponent>();
    }

    void *ScriptGlue::Entity_CreateComponent(uint64_t id, MonoReflectionType *reflectionType)
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

    bool ScriptGlue::Entity_HasComponent(uint64_t id, MonoReflectionType *reflectionType)
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

    void ScriptGlue::Entity_RemoveComponent(uint64_t id, MonoReflectionType *reflectionType)
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

    void *ScriptGlue::Entity_GetComponent(uint64_t id, MonoReflectionType *reflectionType)
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

    MonoObject *ScriptGlue::Entity_GetScriptInstance(uint64_t id)
    {
        return ScriptingEngine::GetEntityScriptInstance(id)->GetMObject().GetMonoObject();
    }

    uint64_t ScriptGlue::Entity_FindEntityByName(MonoString *name)
    {
        MonoError error {};
        char* nameStr = mono_string_to_utf8_checked(name, &error);
        if(error.error_code != MONO_ERROR_NONE)
        {
            BeeError("Could not convert MonoString to char*: {}", mono_error_get_message(&error));
            return 0;
        }
        auto* scene = ScriptingEngine::GetSceneContext();
        auto entity = scene->GetEntityByName(nameStr);
        if(!entity)
        {
            BeeCoreTrace("Could not find entity with name {}", nameStr);
            mono_free(nameStr);
            return 0;
        }
        UUID uuid = entity.GetUUID();
        BeeCoreTrace("Found entity with name {} and id {}", nameStr, uuid);
        mono_free(nameStr);
        return uuid;
    }

    void ScriptGlue::Entity_Destroy(uint64_t id)
    {
        /*DeletionQueue::Frame().PushFunction([id]()
        {
            auto* scene = ScriptingEngine::GetSceneContext();
            if(!scene)
                return;
            auto entity = scene->GetEntityByUUID(id);
            if(!entity)
                return;
            scene->DestroyEntity(entity);
        });*/
        auto* scene = ScriptingEngine::GetSceneContext();
        auto entity = scene->GetEntityByUUID(id);
        scene->DestroyEntity(entity);
    }

    MonoString *ScriptGlue::TextRendererComponent_GetText(uint64_t id)
    {
        auto entity = GetEntity(id);
        MonoString* string = mono_string_new(mono_domain_get(), entity.GetComponent<TextRendererComponent>().Text.c_str());
        return string;
    }

    void ScriptGlue::TextRendererComponent_SetText(uint64_t id, MonoString *text)
    {
        MonoError error {};
        char* textStr = mono_string_to_utf8_checked(text, &error);
        if(error.error_code != MONO_ERROR_NONE)
        {
            BeeError("Could not convert MonoString to char*: {}", mono_error_get_message(&error));
            return;
        }
        auto entity = GetEntity(id);
        if(!entity)
        {
            BeeError("Could not find entity with id {}", id);
            mono_free(textStr);
            return;
        }
        entity.GetComponent<TextRendererComponent>().Text = textStr;
    }
}
