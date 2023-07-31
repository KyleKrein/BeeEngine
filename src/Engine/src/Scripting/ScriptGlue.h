//
// Created by alexl on 30.07.2023.
//

#pragma once
#include "mono/metadata/appdomain.h"
#include "vec3.hpp"
#include "Core/UUID.h"
#include "KeyCodes.h"
#include "Scene/Components.h"

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
        static void RegisterComponent(ComponentGroup<Component...>);

        static void Log_Warn(MonoString* message);
        static void Log_Info(MonoString* message);
        static void Log_Error(MonoString* message);
        static void Log_Trace(MonoString* message);
        static void* Entity_GetTransformComponent(UUID id);
        static void* Entity_CreateComponent(UUID id, MonoReflectionType* reflectionType);
        static bool Entity_HasComponent(UUID id, MonoReflectionType* reflectionType);
        static void Entity_RemoveComponent(UUID id, MonoReflectionType* reflectionType);
        static void* Entity_GetComponent(UUID id, MonoReflectionType* reflectionType);
        static void Entity_GetTranslation(UUID id, glm::vec3* outTranslation);
        static void Entity_SetTranslation(UUID id, glm::vec3* inTranslation);
        static bool Input_IsKeyDown(Key key);
        static bool Input_IsMouseButtonDown(MouseButton button);
    };
}
