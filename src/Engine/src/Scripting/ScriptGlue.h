//
// Created by alexl on 30.07.2023.
//

#pragma once
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
        struct ArrayInfo
        {
            void* data;
            uint64_t size;
        };
        static inline class Entity GetEntity(UUID id);
        template<typename ...Component>
        static void RegisterComponent();
        template<typename ...Component>
        static void RegisterComponent(TypeSequence<Component...>);

        static void Log_Warn(void* message);
        static void Log_Info(void* message);
        static void Log_Error(void* message);
        static void Log_Trace(void* message);
        static void* Entity_GetTransformComponent(uint64_t id);
        //static void* Entity_CreateComponent(uint64_t id, MonoReflectionType* reflectionType);
        //static int32_t Entity_HasComponent(uint64_t id, MonoReflectionType* reflectionType);
        //static void Entity_RemoveComponent(uint64_t id, MonoReflectionType* reflectionType);
        //static void* Entity_GetComponent(uint64_t id, MonoReflectionType* reflectionType);
        static void Entity_GetTranslation(uint64_t id, glm::vec3* outTranslation);
        static void Entity_SetTranslation(uint64_t id, glm::vec3* inTranslation);
        static uint64_t Entity_GetParent(uint64_t id);
        static void Entity_SetParent(uint64_t childId, uint64_t parentId);
        static uint64_t Entity_GetNextChild(uint64_t id, uint64_t prevChildId);
        static int32_t Entity_HasChild(uint64_t parentId, uint64_t childId);
        static void Entity_AddChild(uint64_t parentId, uint64_t childId);
        static void Entity_RemoveChild(uint64_t parentId, uint64_t childId);
        static void* Entity_GetName(uint64_t id);
        static void Entity_SetName(uint64_t id, void* name);
        static void Entity_Destroy(uint64_t id);
        static uint64_t Entity_Duplicate(uint64_t id);
        static uint64_t Entity_InstantiatePrefab(AssetHandle* handle, uint64_t parentId);
        static void * TextRendererComponent_GetText(uint64_t id);
        static void TextRendererComponent_SetText(uint64_t id, void* text);
        static uint64_t Entity_FindEntityByName(void* name);
        static int32_t Input_IsKeyDown(Key key);
        static int32_t Input_IsMouseButtonDown(MouseButton button);
        static void Input_GetMousePosition(glm::vec2* outPosition);
        static void Input_GetMousePositionInWorldSpace(uint64_t id, glm::vec2* outPosition);
        static void Asset_Load(AssetHandle* handle);
        static void Asset_Unload(AssetHandle* handle);
        static int32_t Asset_IsLoaded(AssetHandle* handle);
        static int32_t Asset_IsValid(AssetHandle* handle);
        static uint64_t Physics2D_CastRay(glm::vec2* start, glm::vec2* end);
        static void * Locale_GetLocale();
        static void Locale_SetLocale(void* locale);
        static void * Locale_TranslateStatic(void* key);
        static void * Locale_TranslateDynamic(void* key, ArrayInfo args);
    };
}
