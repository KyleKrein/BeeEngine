//
// Created by alexl on 30.07.2023.
//

#include "ScriptGlue.h"
#include "Core/Application.h"
#include "Core/AssetManagement/Asset.h"
#include "Core/AssetManagement/AssetManager.h"
#include "Core/Casts.h"
#include "Core/Input.h"
#include "Core/Logging/GameLogger.h"
#include "Core/Logging/Log.h"
#include "Core/Numbers.h"
#include "JobSystem/SpinLock.h"
#include "MAssembly.h"
#include "NativeToManaged.h"
#include "Renderer/BindingSet.h"
#include "Renderer/CommandBuffer.h"
#include "Renderer/FrameBuffer.h"
#include "Renderer/IBindable.h"
#include "Renderer/RenderingQueue.h"
#include "Renderer/Texture.h"
#include "Renderer/UniformBuffer.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Prefab.h"
#include "Scene/Scene.h"
#include "Scripting/ScriptingEngine.h"
#include "ScriptingEngine.h"
#include <cstdint>
#include <exception>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

#define BEE_NATIVE_FUNCTION(name) ScriptingEngine::RegisterNativeFunction(#name, (void*)&name)

namespace BeeEngine
{

    std::unordered_map<ScriptGlue::ComponentType, std::function<void*(Entity)>> s_CreateComponentFunctions;
    std::unordered_map<ScriptGlue::ComponentType, std::function<void*(Entity)>> s_GetComponentFunctions;
    std::unordered_map<ScriptGlue::ComponentType, std::function<bool(Entity)>> s_HasComponentFunctions;
    std::unordered_map<ScriptGlue::ComponentType, std::function<void(Entity)>> s_RemoveComponentFunctions;

    std::optional<ScriptGlue::ComponentType> ManagedNameToComponentType(const String& name)
    {
        if (name == "BeeEngine.TransformComponent")
            return ScriptGlue::ComponentType::Transform;
        if (name == "BeeEngine.SpriteRendererComponent")
            return ScriptGlue::ComponentType::SpriteRenderer;
        if (name == "BeeEngine.TextRendererComponent")
            return ScriptGlue::ComponentType::TextRenderer;
        if (name == "BeeEngine.BoxCollider2DComponent")
            return ScriptGlue::ComponentType::BoxCollider2D;
        if (name == "BeeEngine.Rigidbody2DComponent")
            return ScriptGlue::ComponentType::Rigidbody2D;
        if (name == "BeeEngine.CircleRendererComponent")
            return ScriptGlue::ComponentType::CircleRenderer;
        return std::nullopt;
    }

    template <typename... Component>
    void ScriptGlue::RegisterComponent()
    {
        (
            []()
            {
                auto typeName = TypeName<Component>();
                size_t pos = typeName.find_last_of(':');
                String managedTypeName = FormatString("BeeEngine.{}", typeName.substr(pos + 1));
                std::optional<ScriptGlue::ComponentType> managedType = ManagedNameToComponentType(managedTypeName);
                if (!managedType.has_value())
                {
                    BeeCoreTrace("Could not find C# component type {}", managedTypeName);
                    return;
                }
                s_CreateComponentFunctions[managedType.value()] = [](Entity entity)
                { return &entity.AddComponent<Component>(); };
                s_GetComponentFunctions[managedType.value()] = [](Entity entity)
                { return &entity.GetComponent<Component>(); };
                s_HasComponentFunctions[managedType.value()] = [](Entity entity)
                { return entity.HasComponent<Component>(); };
                s_RemoveComponentFunctions[managedType.value()] = [](Entity entity)
                { entity.RemoveComponent<Component>(); };
            }(),
            ...);
    }

    template <typename... Component>
    void ScriptGlue::RegisterComponent(TypeSequence<Component...>)
    {
        RegisterComponent<Component...>();
    }
    void ScriptGlue::Register()
    {
        if (!s_CreateComponentFunctions.empty())
        {
            s_CreateComponentFunctions.clear();
            s_GetComponentFunctions.clear();
            s_HasComponentFunctions.clear();
            s_RemoveComponentFunctions.clear();
        }
        {
            RegisterComponent(AllComponents{});
        }
        { // Internal Calls
            BEE_NATIVE_FUNCTION(Log_Warn);
            BEE_NATIVE_FUNCTION(Log_Info);
            BEE_NATIVE_FUNCTION(Log_Error);
            BEE_NATIVE_FUNCTION(Log_Trace);

            BEE_NATIVE_FUNCTION(Entity_GetComponent);
            BEE_NATIVE_FUNCTION(Entity_CreateComponent);
            BEE_NATIVE_FUNCTION(Entity_HasComponent);
            BEE_NATIVE_FUNCTION(Entity_RemoveComponent);
            BEE_NATIVE_FUNCTION(Entity_FindEntityByName);
            BEE_NATIVE_FUNCTION(Entity_GetParent);
            BEE_NATIVE_FUNCTION(Entity_SetParent);
            BEE_NATIVE_FUNCTION(Entity_GetNextChild);
            BEE_NATIVE_FUNCTION(Entity_HasChild);
            BEE_NATIVE_FUNCTION(Entity_AddChild);
            BEE_NATIVE_FUNCTION(Entity_RemoveChild);
            BEE_NATIVE_FUNCTION(Entity_GetName);
            BEE_NATIVE_FUNCTION(Entity_SetName);
            BEE_NATIVE_FUNCTION(Entity_Destroy);
            BEE_NATIVE_FUNCTION(Entity_Duplicate);
            BEE_NATIVE_FUNCTION(Entity_InstantiatePrefab);
            BEE_NATIVE_FUNCTION(Entity_GetEnttID);

            BEE_NATIVE_FUNCTION(TextRendererComponent_GetText);
            BEE_NATIVE_FUNCTION(TextRendererComponent_SetText);

            BEE_NATIVE_FUNCTION(Input_IsKeyDown);
            BEE_NATIVE_FUNCTION(Input_IsMouseButtonDown);
            BEE_NATIVE_FUNCTION(Input_GetMousePosition);
            BEE_NATIVE_FUNCTION(Input_GetMouseWheelDelta);
            BEE_NATIVE_FUNCTION(Input_GetMousePositionInWorldSpace);

            BEE_NATIVE_FUNCTION(Asset_Load);
            BEE_NATIVE_FUNCTION(Asset_Unload);
            BEE_NATIVE_FUNCTION(Asset_IsValid);
            BEE_NATIVE_FUNCTION(Asset_IsLoaded);

            BEE_NATIVE_FUNCTION(Physics2D_CastRay);

            BEE_NATIVE_FUNCTION(Locale_GetLocale);
            BEE_NATIVE_FUNCTION(Locale_SetLocale);
            BEE_NATIVE_FUNCTION(Locale_TranslateStatic);
            BEE_NATIVE_FUNCTION(Locale_TranslateDynamic);

            BEE_NATIVE_FUNCTION(Scene_GetActive);
            BEE_NATIVE_FUNCTION(Scene_SetActive);

            BEE_NATIVE_FUNCTION(Renderer_SubmitInstance);
            BEE_NATIVE_FUNCTION(Renderer_SubmitText);

            BEE_NATIVE_FUNCTION(Framebuffer_CreateDefault);
            BEE_NATIVE_FUNCTION(Framebuffer_Resize);
            BEE_NATIVE_FUNCTION(Framebuffer_Bind);
            BEE_NATIVE_FUNCTION(Framebuffer_Unbind);
            BEE_NATIVE_FUNCTION(Framebuffer_Destroy);
            BEE_NATIVE_FUNCTION(UniformBuffer_CreateDefault);
            BEE_NATIVE_FUNCTION(UniformBuffer_SetData);
            BEE_NATIVE_FUNCTION(UniformBuffer_Destroy);
            BEE_NATIVE_FUNCTION(BindingSet_Create);
            BEE_NATIVE_FUNCTION(BindingSet_Destroy);
        }
    }
    void ScriptGlue::Log_Warn(void* message)
    {
        auto msg = NativeToManaged::StringGetFromManagedString(message);
        BeeWarn(msg);
    }

    void ScriptGlue::Log_Info(void* message)
    {
        auto msg = NativeToManaged::StringGetFromManagedString(message);
        BeeInfo(msg);
    }

    void ScriptGlue::Log_Error(void* message)
    {
        auto msg = NativeToManaged::StringGetFromManagedString(message);
        BeeError(msg);
    }

    void ScriptGlue::Log_Trace(void* message)
    {
        auto msg = NativeToManaged::StringGetFromManagedString(message);
        BeeCoreTrace(msg);
    }

    void ScriptGlue::Entity_GetTranslation(uint64_t id, glm::vec3* outTranslation)
    {
        auto* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(id);
        *outTranslation = entity.GetComponent<TransformComponent>().Translation;
    }

    void ScriptGlue::Entity_SetTranslation(uint64_t id, glm::vec3* inTranslation)
    {
        auto* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(id);
        entity.GetComponent<TransformComponent>().Translation = *inTranslation;
    }

    int32_t ScriptGlue::Input_IsKeyDown(Key key)
    {
        return Input::KeyPressed(key);
    }
    void ScriptGlue::Input_GetMouseWheelDelta(glm::vec2* outWheelDelta)
    {
        *outWheelDelta = {Input::GetMouseWheelX(), Input::GetMouseWheelY()};
    }

    int32_t ScriptGlue::Input_IsMouseButtonDown(MouseButton button)
    {
        return Input::MouseKeyPressed(button);
    }

    void* ScriptGlue::Entity_GetTransformComponent(uint64_t id)
    {
        auto* scene = ScriptingEngine::GetSceneContext();
        Entity entity = scene->GetEntityByUUID(id);
        return &entity.GetComponent<TransformComponent>();
    }
    class Entity ScriptGlue::GetEntity(UUID id)
    {
        auto* scene = ScriptingEngine::GetSceneContext();
        return scene->GetEntityByUUID(id);
    }

    void* ScriptGlue::Entity_CreateComponent(uint64_t id, ComponentType componentType)
    {
        auto entity = GetEntity(id);
        BeeExpects(entity);
        return s_CreateComponentFunctions.at(componentType)(entity);
    }

    int32_t ScriptGlue::Entity_HasComponent(uint64_t id, ComponentType componentType)
    {
        auto entity = GetEntity(id);
        BeeExpects(entity);
        return s_HasComponentFunctions.at(componentType)(entity);
    }

    void ScriptGlue::Entity_RemoveComponent(uint64_t id, ComponentType componentType)
    {
        auto entity = GetEntity(id);
        BeeExpects(entity);
        s_RemoveComponentFunctions.at(componentType)(entity);
    }

    void* ScriptGlue::Entity_GetComponent(uint64_t id, ComponentType componentType)
    {
        auto entity = GetEntity(id);
        BeeExpects(entity);
        return s_GetComponentFunctions.at(componentType)(entity);
    }

    uint64_t ScriptGlue::Entity_FindEntityByName(void* name)
    {
        auto nameStr = NativeToManaged::StringGetFromManagedString(name);
        auto* scene = ScriptingEngine::GetSceneContext();
        auto entity = scene->GetEntityByName(std::string_view(nameStr));
        if (!entity)
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
        auto* scene = ScriptingEngine::GetSceneContext();
        auto entity = scene->GetEntityByUUID(id);
        scene->DestroyEntity(entity);
    }

    void* ScriptGlue::TextRendererComponent_GetText(uint64_t id)
    {
        auto entity = GetEntity(id);
        GCHandle handle = NativeToManaged::StringCreateManaged(entity.GetComponent<TextRendererComponent>().Text);
        return handle;
    }

    void ScriptGlue::TextRendererComponent_SetText(uint64_t id, void* text)
    {
        auto textStr = NativeToManaged::StringGetFromManagedString(text);
        auto entity = GetEntity(id);
        if (!entity)
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
        if (AssetManager::IsAssetLoaded(*handle))
        {
            AssetManager::UnloadAsset(*handle);
        }
    }

    int32_t ScriptGlue::Asset_IsLoaded(AssetHandle* handle)
    {
        return AssetManager::IsAssetLoaded(*handle);
    }

    int32_t ScriptGlue::Asset_IsValid(AssetHandle* handle)
    {
        return AssetManager::IsAssetHandleValid(*handle);
    }

    uint64_t ScriptGlue::Entity_GetParent(uint64_t id)
    {
        auto entity = GetEntity(id);
        auto parent = entity.GetParent();
        if (parent)
        {
            return parent.GetUUID();
        }
        return 0;
    }

    void ScriptGlue::Entity_SetParent(uint64_t childId, uint64_t parentId)
    {
        auto child = GetEntity(childId);
        if (parentId == 0)
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
        if (prevChildId == 0)
        {
            if (children.empty())
                return 0;
            return children.at(0).GetUUID();
        }
        for (size_t i = 0; i < children.size(); i++)
        {
            if (children.at(i).GetUUID() == prevChildId)
            {
                if (i + 1 < children.size())
                    return children.at(i + 1).GetUUID();
                return 0;
            }
        }
        return 0;
    }

    int32_t ScriptGlue::Entity_HasChild(uint64_t parentId, uint64_t childId)
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

    void* ScriptGlue::Entity_GetName(uint64_t id)
    {
        auto entity = GetEntity(id);
        GCHandle handle = NativeToManaged::StringCreateManaged(entity.GetComponent<TagComponent>().Tag);
        return handle;
    }

    void ScriptGlue::Entity_SetName(uint64_t id, void* name)
    {
        auto nameStr = NativeToManaged::StringGetFromManagedString(name);
        auto entity = GetEntity(id);
        if (!entity)
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

    uint64_t ScriptGlue::Entity_InstantiatePrefab(AssetHandle* handlePtr, uint64_t parentId)
    {
        auto* scene = ScriptingEngine::GetSceneContext();
        auto parentEntity = parentId == 0 ? Entity::Null : scene->GetEntityByUUID(parentId);
        auto& prefab = AssetManager::GetAsset<Prefab>(*handlePtr);
        auto entity = scene->InstantiatePrefab(prefab, parentEntity);
        return entity.GetUUID();
    }

    uint64_t ScriptGlue::Physics2D_CastRay(glm::vec2* start, glm::vec2* end)
    {
        auto* scene = ScriptingEngine::GetSceneContext();
        auto result = scene->RayCast2D(*start, *end);
        if (!result)
            return 0;
        return result.GetUUID();
    }

    void ScriptGlue::Input_GetMousePosition(glm::vec2* outPosition)
    {
        *outPosition = ScriptingEngine::GetMousePosition();
    }

    void ScriptGlue::Input_GetMousePositionInWorldSpace(uint64_t id, glm::vec2* outPosition)
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

    void* ScriptGlue::Locale_GetLocale()
    {
        GCHandle handle = NativeToManaged::StringCreateManaged(ScriptingEngine::GetScriptingLocale());
        return handle;
    }

    void ScriptGlue::Locale_SetLocale(void* locale)
    {
        auto localeStr = NativeToManaged::StringGetFromManagedString(locale);
        ScriptingEngine::GetLocaleDomain().SetLocale(localeStr);
    }

    void* ScriptGlue::Locale_TranslateStatic(void* key)
    {
        auto keyStr = NativeToManaged::StringGetFromManagedString(key);
        String translated = ScriptingEngine::GetLocaleDomain().Translate(keyStr.c_str());
        GCHandle handle = NativeToManaged::StringCreateManaged(translated);
        return handle;
    }
    using VariantType =
        std::variant<String, bool, int32_t, int16_t, int64_t, uint32_t, uint16_t, uint64_t, float32_t, float64_t>;

    /**
     * @brief Struct that holds information about the type of the argument
     * and the pointer to the argument or the data itself if it is a value type,
     * that is less than 8 bytes
     *
     * IMPORTANT: If this type is changed, the corresponding C# type in
     * InternalCalls.cs MUST be changed as well
     */
    struct ReflectionTypeInfo
    {
        MType Type;
        void* Ptr;
    };

    void* ScriptGlue::Locale_TranslateDynamic(void* keyM, ArrayInfo argsM)
    {
        String key = NativeToManaged::StringGetFromManagedString(keyM);
        std::vector<VariantType> args;
        std::span<ReflectionTypeInfo> argsSpan{reinterpret_cast<ReflectionTypeInfo*>(argsM.data), argsM.size};
        args.reserve(argsM.size);

        for (auto& arg : argsSpan)
        {
            switch (arg.Type)
            {
                case MType::String:
                    args.push_back(NativeToManaged::StringGetFromManagedString(arg.Ptr));
                    break;
                case MType::Boolean:
                    args.push_back(*reinterpret_cast<bool32_t*>(&arg.Ptr) != 0);
                    break;
                case MType::Int32:
                    args.push_back(*reinterpret_cast<int32_t*>(&arg.Ptr));
                    break;
                case MType::Int16:
                    args.push_back(*reinterpret_cast<int16_t*>(&arg.Ptr));
                    break;
                case MType::Int64:
                    args.push_back(*reinterpret_cast<int64_t*>(&arg.Ptr));
                    break;
                case MType::UInt32:
                    args.push_back(*reinterpret_cast<uint32_t*>(&arg.Ptr));
                    break;
                case MType::UInt16:
                    args.push_back(*reinterpret_cast<uint16_t*>(&arg.Ptr));
                    break;
                case MType::UInt64:
                    args.push_back(*reinterpret_cast<uint64_t*>(&arg.Ptr));
                    break;
                case MType::Single:
                    args.push_back(*reinterpret_cast<float32_t*>(&arg.Ptr));
                    break;
                case MType::Double:
                    args.push_back(*reinterpret_cast<float64_t*>(&arg.Ptr));
                    break;
                case MType::Byte:
                    args.push_back(*reinterpret_cast<uint8_t*>(&arg.Ptr));
                    break;
                case MType::SByte:
                    args.push_back(*reinterpret_cast<int8_t*>(&arg.Ptr));
                    break;
                default:
                    BeeError("Unsupported argument type {}", arg.Type);
                    break;
            }
        }

        String translated = ScriptingEngine::GetLocaleDomain().TranslateRuntime(key.c_str(), args);
        BeeCoreTrace("Translated key {} to {}", key, translated);
        return NativeToManaged::StringCreateManaged(translated);
    }

    void* ScriptGlue::Scene_GetActive()
    {
        return &ScriptingEngine::GetSceneContext()->Handle;
    }
    void ScriptGlue::Scene_SetActive(void* scene)
    {
        AssetHandle handle = *static_cast<AssetHandle*>(scene);
        Application::SubmitToMainThread([handle]() { ScriptingEngine::RequestSceneChange(handle); });
    }

    struct ScriptGlue::ScriptGlueInternalState
    {
        std::unordered_map<ScriptGlue::ModelType, Model*> Models;
        BindingSet* BlankTextureSet = nullptr;
        std::unordered_map<FrameBuffer*, Scope<FrameBuffer>> AllocatedFramebuffers;
        Jobs::SpinLock AllocatedFramebuffersLock;
        std::unordered_map<UniformBuffer*, Scope<UniformBuffer>> AllocatedUniformBuffers;
        Jobs::SpinLock AllocatedUniformBuffersLock;
        std::unordered_map<BindingSet*, Scope<BindingSet>> AllocatedBindingSets;
        Jobs::SpinLock AllocatedBindingSetsLock;
    };

    BindingSet* ScriptGlue::GetBindingSetForModelType(ScriptGlue::ModelType modelType, AssetHandle* handle)
    {
        BindingSet* bindingSet = s_Data->BlankTextureSet;
        switch (modelType)
        {
            case ScriptGlue::ModelType::Rectangle:
            {
                if (handle != nullptr && *handle != AssetHandle{0, 0})
                {
                    bindingSet = &AssetManager::GetAsset<Texture2D>(*handle, ScriptingEngine::GetScriptingLocale())
                                      .GetBindingSet();
                }
                break;
            }
            case ModelType::Line:
                break;
            case ScriptGlue::ModelType::Circle:
            {
                break;
            }
            case ScriptGlue::ModelType::Text:
            {
                bindingSet =
                    &AssetManager::GetAsset<Font>(*handle, ScriptingEngine::GetScriptingLocale()).GetAtlasBindingSet();
                break;
            }
            default:
                BeeCoreAssert(false, "Unknown model type: {}", modelType);
                std::terminate();
        }
        return bindingSet;
    }

    void ScriptGlue::Renderer_SubmitInstance(
        BindingSet* cameraBindingSet, CommandBuffer cmd, ModelType modelType, AssetHandle* handle, ArrayInfo data)
    {
        Model& model = *s_Data->Models[modelType];
        BindingSet* bindingSet;
        if (modelType == ModelType::Framebuffer)
        {
            struct FramebufferData
            {
                glm::mat4 Model;
                FrameBuffer* Framebuffer;
            };
            FrameBuffer* framebuffer = static_cast<FramebufferData*>(data.data)->Framebuffer;
            bindingSet = &framebuffer->GetColorBindingSet();
        }
        else
        {
            bindingSet = GetBindingSetForModelType(modelType, handle);
        }
        std::vector<BindingSet*> bindingSets = {
            cameraBindingSet ? cameraBindingSet
                             : ScriptingEngine::GetSceneContext()->GetSceneRendererData().CameraBindingSet.get(),
            bindingSet};
        cmd.SubmitInstance(model, bindingSets, {(byte*)data.data, data.size});
    }

    void ScriptGlue::Renderer_SubmitText(BindingSet* cameraBindingSet,
                                         CommandBuffer cmd,
                                         AssetHandle* handle,
                                         void* textPtr,
                                         glm::mat4* transform,
                                         TextRenderingConfiguration* config,
                                         int32_t entityId)
    {
        String text = NativeToManaged::StringGetFromManagedString(textPtr);
        Model& model = *s_Data->Models[ModelType::Text];
        Font& font = AssetManager::GetAsset<Font>(*handle, ScriptingEngine::GetScriptingLocale());
        cmd.DrawString(text,
                       font,
                       cameraBindingSet ? *cameraBindingSet
                                        : *ScriptingEngine::GetSceneContext()->GetSceneRendererData().CameraBindingSet,
                       *transform,
                       *config,
                       entityId);
    }

    uint64_t ScriptGlue::Entity_GetEnttID(uint64_t uuid)
    {
        Entity entity = GetEntity(uuid);
        BeeCoreTrace("Entity_GetEnttID");
        return static_cast<uint64_t>(static_cast<uint32_t>(entity));
    }

    FrameBuffer* ScriptGlue::Framebuffer_CreateDefault(uint32_t width, uint32_t height, Color4 clearColor)
    {
        FrameBufferPreferences preferences;
        preferences.Width = width;   // * WindowHandler::GetInstance()->GetScaleFactor();
        preferences.Height = height; // * WindowHandler::GetInstance()->GetScaleFactor();
        preferences.Attachments = {
            FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::UINT64, FrameBufferTextureFormat::Depth24};

        // preferences.Attachments.Attachments[1].TextureUsage = FrameBufferTextureUsage::CPUAndGPU; // RedInteger
        preferences.Attachments.Attachments[0].ClearColor = clearColor;
        auto framebuffer = FrameBuffer::Create(preferences);
        FrameBuffer* result = framebuffer.get();
        std::unique_lock lock(s_Data->AllocatedFramebuffersLock);
        s_Data->AllocatedFramebuffers[result] = std::move(framebuffer);
        return result;
    }
    void ScriptGlue::Framebuffer_Resize(FrameBuffer* framebuffer, uint32_t width, uint32_t height)
    {
        // width = width * WindowHandler::GetInstance()->GetScaleFactor();
        // height = height * WindowHandler::GetInstance()->GetScaleFactor();
        framebuffer->Resize(width, height);
    }
    void ScriptGlue::Framebuffer_Destroy(FrameBuffer* framebuffer)
    {
        std::unique_lock lock(s_Data->AllocatedFramebuffersLock);
        s_Data->AllocatedFramebuffers.erase(framebuffer);
    }
    void ScriptGlue::Framebuffer_Bind(FrameBuffer* framebuffer, CommandBuffer* cmd)
    {
        *cmd = framebuffer->Bind();
    }
    void ScriptGlue::Framebuffer_Unbind(FrameBuffer* framebuffer, CommandBuffer* cmd)
    {
        framebuffer->Unbind(*cmd);
    }

    UniformBuffer* ScriptGlue::UniformBuffer_CreateDefault(uint32_t sizeBytes)
    {
        auto uniformBuffer = UniformBuffer::Create(sizeBytes);
        UniformBuffer* result = uniformBuffer.get();
        std::unique_lock lock(s_Data->AllocatedUniformBuffersLock);
        s_Data->AllocatedUniformBuffers[result] = std::move(uniformBuffer);
        return result;
    }
    void ScriptGlue::UniformBuffer_Destroy(UniformBuffer* buffer)
    {
        std::unique_lock lock(s_Data->AllocatedUniformBuffersLock);
        s_Data->AllocatedUniformBuffers.erase(buffer);
    }
    void ScriptGlue::UniformBuffer_SetData(UniformBuffer* buffer, void* data, uint32_t sizeBytes)
    {
        buffer->SetData(data, sizeBytes);
    }
    BindingSet* ScriptGlue::BindingSet_Create(ArrayInfo elements)
    {
        std::vector<BindingSetElement> bindingSetElements;
        bindingSetElements.reserve(elements.size);
        IBindable** bindables = static_cast<IBindable**>(elements.data);
        for (uint32_t i = 0; i < narrow_cast<uint32_t>(elements.size); i++)
        {
            bindingSetElements.push_back({i, *bindables[i]});
        }
        auto bindingSet = BindingSet::Create(BeeMove(bindingSetElements));
        BindingSet* result = bindingSet.get();
        std::unique_lock lock(s_Data->AllocatedBindingSetsLock);
        s_Data->AllocatedBindingSets[result] = std::move(bindingSet);
        return result;
    }
    void ScriptGlue::BindingSet_Destroy(BindingSet* bindingSet)
    {
        std::unique_lock lock(s_Data->AllocatedBindingSetsLock);
        s_Data->AllocatedBindingSets.erase(bindingSet);
    }

    void ScriptGlue::Init()
    {
        auto& assetManager = Application::GetInstance().GetAssetManager();
        s_Data = new ScriptGlueInternalState{
            .Models = {{ModelType::Rectangle, &assetManager.GetModel("Renderer2D_Rectangle")},
                       {ModelType::Circle, &assetManager.GetModel("Renderer2D_Circle")},
                       {ModelType::Text, &assetManager.GetModel("Renderer_Font")},
                       {ModelType::Line, &assetManager.GetModel("Renderer_Line")},
                       {ModelType::Framebuffer, &assetManager.GetModel("Renderer_Framebuffer")}},
            .BlankTextureSet = &assetManager.GetTexture("Blank").GetBindingSet()};
    }

    void ScriptGlue::Shutdown()
    {
        delete s_Data;
    }
} // namespace BeeEngine
