//
// Created by alexl on 30.07.2023.
//

#include "ScriptGlue.h"
#include "ScriptingEngine.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Scene/Prefab.h"
#include "Core/Input.h"
#include "MAssembly.h"
#include "Core/Logging/GameLogger.h"


#define BEE_INTERNAL_CALL(name) ScriptingEngine::RegisterInternalCall("BeeEngine.Internal.InternalCalls::" #name, (void*)&name)

namespace BeeEngine
{
    static UTF8String ConvertMonoStringToString(MonoString* str)
    {
        MonoError error {};
        char* strPtr = mono_string_to_utf8_checked(str, &error);
        if(error.error_code != MONO_ERROR_NONE)
        {
            BeeError("Could not convert MonoString to char*: {}", mono_error_get_message(&error));
            return {};
        }
        UTF8String result = strPtr;
        mono_free(strPtr);
        return result;
    }

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
            std::string managedTypeName = FormatString("BeeEngine.{}", typeName.substr(pos + 1));
            MonoType* managedType = mono_reflection_type_from_name((char*)managedTypeName.c_str(), ScriptingEngine::GetCoreAssembly().m_MonoImage);
            if(!managedType)
            {
                BeeCoreTrace("Could not find C# component type {}", managedTypeName);
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
            BEE_INTERNAL_CALL(Entity_GetParent);
            BEE_INTERNAL_CALL(Entity_SetParent);
            BEE_INTERNAL_CALL(Entity_GetNextChild);
            BEE_INTERNAL_CALL(Entity_HasChild);
            BEE_INTERNAL_CALL(Entity_AddChild);
            BEE_INTERNAL_CALL(Entity_RemoveChild);
            BEE_INTERNAL_CALL(Entity_GetName);
            BEE_INTERNAL_CALL(Entity_SetName);
            BEE_INTERNAL_CALL(Entity_Destroy);
            BEE_INTERNAL_CALL(Entity_Duplicate);
            BEE_INTERNAL_CALL(Entity_InstantiatePrefab);

            BEE_INTERNAL_CALL(Entity_GetTransformComponent);
            BEE_INTERNAL_CALL(Entity_GetTranslation);
            BEE_INTERNAL_CALL(Entity_SetTranslation);

            BEE_INTERNAL_CALL(TextRendererComponent_GetText);
            BEE_INTERNAL_CALL(TextRendererComponent_SetText);

            BEE_INTERNAL_CALL(Input_IsKeyDown);
            BEE_INTERNAL_CALL(Input_IsMouseButtonDown);
            BEE_INTERNAL_CALL(Input_GetMousePosition);
            BEE_INTERNAL_CALL(Input_GetMousePositionInWorldSpace);

            BEE_INTERNAL_CALL(Asset_Load);
            BEE_INTERNAL_CALL(Asset_Unload);
            BEE_INTERNAL_CALL(Asset_IsValid);
            BEE_INTERNAL_CALL(Asset_IsLoaded);

            BEE_INTERNAL_CALL(Physics2D_CastRay);

            BEE_INTERNAL_CALL(Locale_GetLocale);
            BEE_INTERNAL_CALL(Locale_SetLocale);
            BEE_INTERNAL_CALL(Locale_TranslateStatic);
            BEE_INTERNAL_CALL(Locale_TranslateDynamic);
        }
    }

    void ScriptGlue::Log_Warn(MonoString *message)
    {
        auto msg = ConvertMonoStringToString(message);
        BeeWarn(msg);
    }

    void ScriptGlue::Log_Info(MonoString *message)
    {
        auto msg = ConvertMonoStringToString(message);
        BeeInfo(msg);
    }

    void ScriptGlue::Log_Error(MonoString *message)
    {
        auto msg = ConvertMonoStringToString(message);
        BeeError(msg);
    }

    void ScriptGlue::Log_Trace(MonoString *message)
    {
        auto msg = ConvertMonoStringToString(message);
        BeeCoreTrace(msg);
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
        auto nameStr = ConvertMonoStringToString(name);
        auto* scene = ScriptingEngine::GetSceneContext();
        auto entity = scene->GetEntityByName(nameStr);
        if(!entity)
        {
            BeeCoreTrace("Could not find entity with name {}", nameStr);
            return 0;
        }
        UUID uuid = entity.GetUUID();
        BeeCoreTrace("Found entity with name {} and id {}", nameStr, uuid);
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
        auto textStr = ConvertMonoStringToString(text);
        auto entity = GetEntity(id);
        if(!entity)
        {
            BeeError("Could not find entity with id {}", id);
            return;
        }
        entity.GetComponent<TextRendererComponent>().Text = textStr;
    }

    void ScriptGlue::Asset_Load(AssetHandle* handle)
    {
        AssetManager::GetAsset<Asset>(*handle, ScriptingEngine::GetScriptingLocale());
    }

    void ScriptGlue::Asset_Unload(AssetHandle* handle)
    {
        AssetManager::UnloadAsset(*handle);
    }

    bool ScriptGlue::Asset_IsLoaded(AssetHandle* handle)
    {
        return AssetManager::IsAssetHandleValid(*handle);
    }

    bool ScriptGlue::Asset_IsValid(AssetHandle* handle)
    {
        return AssetManager::IsAssetLoaded(*handle);
    }

    uint64_t ScriptGlue::Entity_GetParent(uint64_t id)
    {
        auto entity = GetEntity(id);
        auto parent = entity.GetParent();
        if(parent)
        {
            return parent.GetUUID();
        }
        return 0;
    }

    void ScriptGlue::Entity_SetParent(uint64_t childId, uint64_t parentId)
    {
        auto child = GetEntity(childId);
        if(parentId == 0)
        {
            child.RemoveParent();
            return;
        }
        auto parent = GetEntity(parentId);
        child.SetParent(parent);
    }

    uint64_t ScriptGlue::Entity_GetNextChild(uint64_t id, uint64_t prevChildId)
    {
        auto entity = GetEntity(id);
        auto& children = entity.GetComponent<HierarchyComponent>().Children;
        if(prevChildId == 0)
        {
            if(children.empty())
                return 0;
            return children.at(0).GetUUID();
        }
        for(size_t i = 0; i < children.size(); i++)
        {
            if(children.at(i).GetUUID() == prevChildId)
            {
                if(i + 1 < children.size())
                    return children.at(i + 1).GetUUID();
                return 0;
            }
        }
        return 0;
    }

    bool ScriptGlue::Entity_HasChild(uint64_t parentId, uint64_t childId)
    {
        auto parent = GetEntity(parentId);
        auto child = GetEntity(childId);
        return parent.HasChild(child);
    }

    void ScriptGlue::Entity_AddChild(uint64_t parentId, uint64_t childId)
    {
        auto parent = GetEntity(parentId);
        auto child = GetEntity(childId);
        child.SetParent(parent);
    }

    void ScriptGlue::Entity_RemoveChild(uint64_t parentId, uint64_t childId)
    {
        auto parent = GetEntity(parentId);
        auto child = GetEntity(childId);
        BeeCoreAssert(parent.HasChild(child), "Parent does not have child!");
        child.RemoveParent();
    }

    MonoString *ScriptGlue::Entity_GetName(uint64_t id)
    {
        auto entity = GetEntity(id);
        MonoString* string = mono_string_new(mono_domain_get(), entity.GetComponent<TagComponent>().Tag.c_str());
        return string;
    }

    void ScriptGlue::Entity_SetName(uint64_t id, MonoString *name)
    {
        auto nameStr = ConvertMonoStringToString(name);
        auto entity = GetEntity(id);
        if(!entity)
        {
            BeeError("Could not find entity with id {}", id);
            return;
        }
        entity.GetComponent<TagComponent>().Tag = nameStr;
    }

    uint64_t ScriptGlue::Entity_Duplicate(uint64_t id)
    {
        auto* scene = ScriptingEngine::GetSceneContext();
        auto entity = scene->GetEntityByUUID(id);
        auto newEntity = scene->DuplicateEntity(entity);
        return newEntity.GetUUID();
    }

    uint64_t ScriptGlue::Entity_InstantiatePrefab(AssetHandle *handlePtr, uint64_t parentId)
    {
        auto* scene = ScriptingEngine::GetSceneContext();
        auto parentEntity = parentId == 0 ? Entity::Null : scene->GetEntityByUUID(parentId);
        auto& prefab = AssetManager::GetAsset<Prefab>(*handlePtr);
        auto entity = scene->InstantiatePrefab(prefab, parentEntity);
        return entity.GetUUID();
    }

    uint64_t ScriptGlue::Physics2D_CastRay(glm::vec2 *start, glm::vec2 *end)
    {
        auto* scene = ScriptingEngine::GetSceneContext();
        auto result = scene->RayCast2D(*start, *end);
        if(!result)
            return 0;
        return result.GetUUID();
    }

    void ScriptGlue::Input_GetMousePosition(glm::vec2 *outPosition)
    {
        *outPosition = ScriptingEngine::GetMousePosition();
    }

    void ScriptGlue::Input_GetMousePositionInWorldSpace(uint64_t id, glm::vec2 *outPosition)
    {
        auto* scene = ScriptingEngine::GetSceneContext();
        auto entity = scene->GetEntityByUUID(id);
        auto& transform = entity.GetComponent<TransformComponent>();
        auto& camera = entity.GetComponent<CameraComponent>();
        auto mousePos = ScriptingEngine::GetMousePosition();
        auto viewportSize = ScriptingEngine::GetViewportSize();
        float normalizedMouseX = (2.0f * mousePos.x) / viewportSize.x - 1.0f;
        float normalizedMouseY = 1.0f - (2.0f * mousePos.y) / viewportSize.y;
        glm::mat4 invProj = glm::inverse(camera.Camera.GetProjectionMatrix());
        glm::vec4 clipCoords{normalizedMouseX, normalizedMouseY, 1.0f, 1.0f};
        glm::vec4 eyeCoords = invProj * clipCoords;
        eyeCoords = eyeCoords / eyeCoords.w; // Перспективное деление
        glm::mat4 invView = glm::inverse(transform.GetTransform());
        glm::vec4 worldCoords = invView * glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
        *outPosition = {worldCoords.x, worldCoords.y};
    }

    MonoString *ScriptGlue::Locale_GetLocale()
    {
        return mono_string_new(mono_domain_get(), ScriptingEngine::GetScriptingLocale().c_str());
    }

    void ScriptGlue::Locale_SetLocale(MonoString *locale)
    {
        auto localeStr = ConvertMonoStringToString(locale);
        ScriptingEngine::GetLocaleDomain().SetLocale(localeStr);
    }

    MonoString *ScriptGlue::Locale_TranslateStatic(MonoString *key)
    {
        auto keyStr = ConvertMonoStringToString(key);
        return mono_string_new(mono_domain_get(), ScriptingEngine::GetLocaleDomain().Translate(keyStr.c_str()).c_str());
    }
    using VariantType = std::variant<String, bool, int32_t, int16_t, int64_t, uint32_t, uint16_t, uint64_t, float32_t, float64_t>;
    VariantType ProcessArrayElement(MonoObject* obj)
    {
        MonoClass* klass = mono_object_get_class(obj);
        MonoType* type = mono_class_get_type(klass);
        switch (mono_type_get_type(type))
        {
            case MONO_TYPE_BOOLEAN:
            {
                bool* value = (bool*)mono_object_unbox(obj);
                return *value;
            }
            case MONO_TYPE_I4:
            {
                int32_t* value = (int32_t*)mono_object_unbox(obj);
                return *value;
            }
            case MONO_TYPE_I2:
            {
                int16_t* value = (int16_t*)mono_object_unbox(obj);
                return *value;
            }
            case MONO_TYPE_I8:
            {
                int64_t* value = (int64_t*)mono_object_unbox(obj);
                return *value;
            }
            case MONO_TYPE_U4:
            {
                uint32_t* value = (uint32_t*)mono_object_unbox(obj);
                return *value;
            }
            case MONO_TYPE_U2:
            {
                uint16_t* value = (uint16_t*)mono_object_unbox(obj);
                return *value;
            }
            case MONO_TYPE_U8:
            {
                uint64_t* value = (uint64_t*)mono_object_unbox(obj);
                return *value;
            }
            case MONO_TYPE_R4:
            {
                float32_t* value = (float32_t*)mono_object_unbox(obj);
                return *value;
            }
            case MONO_TYPE_R8:
            {
                float64_t* value = (float64_t*)mono_object_unbox(obj);
                return *value;
            }
            case MONO_TYPE_STRING:
            {
                return ConvertMonoStringToString((MonoString*)obj);
            }
            default:
            {
                MonoObject* ex;
                MonoString* str = mono_object_to_string(obj, &ex);
                if(ex)
                {
                    MonoString * message = mono_object_to_string(ex, nullptr);
                    BeeCoreError("Could not convert object to string: {}", ConvertMonoStringToString(message));
                    return {};
                }
                return ConvertMonoStringToString(str);
            }
        }
    }

    MonoString *ScriptGlue::Locale_TranslateDynamic(MonoString *keyMono, MonoArray *argsMono)
    {
        auto key = ConvertMonoStringToString(keyMono);
        std::vector<VariantType> args;
        size_t length = mono_array_length(argsMono);
        args.reserve(length);
        for(size_t i = 0; i < length; i++)
        {
            MonoObject* obj = mono_array_get(argsMono, MonoObject*, i);
            args.push_back(ProcessArrayElement(obj));
        }
        auto translated = ScriptingEngine::GetLocaleDomain().TranslateRuntime(key.c_str(), args);
        return mono_string_new(mono_domain_get(), translated.c_str());
    }
}
