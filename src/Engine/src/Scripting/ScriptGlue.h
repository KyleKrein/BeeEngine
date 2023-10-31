//
// Created by alexl on 30.07.2023.
//

#pragma once
#include "mono/metadata/appdomain.h"
#include "vec3.hpp"
#include "Core/UUID.h"
#include "KeyCodes.h"
#include "Scene/Components.h"
#include "Core/AssetManagement/AssetManager.h"

namespace BeeEngine
{
    class ScriptGlue
    {
    public:
        static void Register();
    private:
        static inline class Entity GetEntity(UUID id);
        template<typename ...Component>
        static void RegisterComponent();
        template<typename ...Component>
        static void RegisterComponent(TypeSequence<Component...>);

        static void Log_Warn(MonoString* message);
        static void Log_Info(MonoString* message);
        static void Log_Error(MonoString* message);
        static void Log_Trace(MonoString* message);
        static void* Entity_GetTransformComponent(uint64_t id);
        static void* Entity_CreateComponent(uint64_t id, MonoReflectionType* reflectionType);
        static bool Entity_HasComponent(uint64_t id, MonoReflectionType* reflectionType);
        static void Entity_RemoveComponent(uint64_t id, MonoReflectionType* reflectionType);
        static void* Entity_GetComponent(uint64_t id, MonoReflectionType* reflectionType);
        static void Entity_GetTranslation(uint64_t id, glm::vec3* outTranslation);
        static void Entity_SetTranslation(uint64_t id, glm::vec3* inTranslation);
        static uint64_t Entity_GetParent(uint64_t id);
        static void Entity_SetParent(uint64_t childId, uint64_t parentId);
        static uint64_t Entity_GetNextChild(uint64_t id, uint64_t prevChildId);
        static bool Entity_HasChild(uint64_t parentId, uint64_t childId);
        static void Entity_AddChild(uint64_t parentId, uint64_t childId);
        static void Entity_RemoveChild(uint64_t parentId, uint64_t childId);
        static MonoString* Entity_GetName(uint64_t id);
        static void Entity_SetName(uint64_t id, MonoString* name);
        static void Entity_Destroy(uint64_t id);
        static uint64_t Entity_Duplicate(uint64_t id);
        static uint64_t Entity_InstantiatePrefab(AssetHandle* handle, uint64_t parentId);
        static MonoString * TextRendererComponent_GetText(uint64_t id);
        static void TextRendererComponent_SetText(uint64_t id, MonoString* text);
        static uint64_t Entity_FindEntityByName(MonoString* name);
        static MonoObject * Entity_GetScriptInstance(uint64_t id);
        static bool Input_IsKeyDown(Key key);
        static bool Input_IsMouseButtonDown(MouseButton button);
        static void Asset_Load(AssetHandle* handle);
        static void Asset_Unload(AssetHandle* handle);
        static bool Asset_IsLoaded(AssetHandle* handle);
        static bool Asset_IsValid(AssetHandle* handle);
    };
}
