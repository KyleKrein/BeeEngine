//
// Created by alexl on 30.07.2023.
//

#pragma once
#include "Core/AssetManagement/AssetManager.h"
#include "Core/UUID.h"
#include "KeyCodes.h"
#include "Renderer/CommandBuffer.h"
#include "Renderer/FrameBuffer.h"
#include "Renderer/UniformBuffer.h"
#include "Scene/Components.h"
#include "vec3.hpp"
#include <cstdint>

namespace BeeEngine
{
    class ScriptGlue
    {
    public:
        static void Init();
        static void Shutdown();
        static void Register();
        /**
         * @brief Component type enum to be sent to and from C# side
         *
         * IMPORTANT: If you change this enum, you must also change
         * the corresponding enum in InternalCalls.cs and
         * add the corresponding case in the switch statement in
         * ScriptGlue.cpp
         */
        enum class ComponentType : uint32_t
        {
            Transform = 0x00,
            SpriteRenderer = 0x01,
            TextRenderer = 0x02,
            BoxCollider2D = 0x03,
            Rigidbody2D = 0x04,
            CircleRenderer = 0x05,
        };
        /**
         * @brief Model type enum to be sent to and from C# side
         *
         * IMPORTANT: If you change this enum, you must also change
         * the corresponding enum in InternalCalls.cs and
         * add the corresponding case in the switch statement in
         * ScriptGlue.cpp
         */
        enum class ModelType : uint32_t
        {
            Rectangle,
            Circle,
            Text,
            Line,
            Framebuffer
        };

    private:
        struct ArrayInfo
        {
            void* data;
            uint64_t size;
        };
        static inline class Entity GetEntity(UUID id);
        template <typename... Component>
        static void RegisterComponent();
        template <typename... Component>
        static void RegisterComponent(TypeSequence<Component...>);

        static void Log_Warn(void* message);
        static void Log_Info(void* message);
        static void Log_Error(void* message);
        static void Log_Trace(void* message);
        static void* Entity_GetTransformComponent(uint64_t id);
        static void* Entity_CreateComponent(uint64_t id, ComponentType componentType);
        static int32_t Entity_HasComponent(uint64_t id, ComponentType componentType);
        static void Entity_RemoveComponent(uint64_t id, ComponentType componentType);
        static void* Entity_GetComponent(uint64_t id, ComponentType componentType);
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
        static uint64_t Entity_GetEnttID(uint64_t uuid);
        static void* TextRendererComponent_GetText(uint64_t id);
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
        static void* Locale_GetLocale();
        static void Locale_SetLocale(void* locale);
        static void* Locale_TranslateStatic(void* key);
        static void* Locale_TranslateDynamic(void* key, ArrayInfo args);
        static void* Scene_GetActive();
        static void Scene_SetActive(void* scene);
        static void Renderer_SubmitInstance(
            BindingSet* cameraBindingSet, CommandBuffer cmd, ModelType modelType, AssetHandle* handle, ArrayInfo data);
        static void Renderer_SubmitText(BindingSet* cameraBindingSet,
                                        CommandBuffer cmd,
                                        AssetHandle* handle,
                                        void* textPtr,
                                        glm::mat4* transform,
                                        TextRenderingConfiguration* config,
                                        int32_t entityId);

        static BindingSet* GetBindingSetForModelType(ModelType modelType, AssetHandle* handle);

        static FrameBuffer* Framebuffer_CreateDefault(uint32_t width, uint32_t height, Color4 clearColor);
        static void Framebuffer_Resize(FrameBuffer* framebuffer, uint32_t width, uint32_t height);
        static void Framebuffer_Destroy(FrameBuffer* framebuffer);
        static void Framebuffer_Bind(FrameBuffer* framebuffer, CommandBuffer* cmd);
        static void Framebuffer_Unbind(FrameBuffer* framebuffer, CommandBuffer* cmd);
        static UniformBuffer* UniformBuffer_CreateDefault(uint32_t sizeBytes);
        static void UniformBuffer_Destroy(UniformBuffer* buffer);
        static void UniformBuffer_SetData(UniformBuffer* buffer, void* data, uint32_t sizeBytes);
        static BindingSet* BindingSet_Create(ArrayInfo elements);
        static void BindingSet_Destroy(BindingSet* bindingSet);

    private:
        struct ScriptGlueInternalState;
        static inline ScriptGlueInternalState* s_Data = nullptr;
    };
} // namespace BeeEngine
