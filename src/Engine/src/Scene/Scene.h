//
// Created by alexl on 02.06.2023.
//

#pragma once

#include "Renderer/EditorCamera.h"
#include "Renderer/Model.h"
#include "Renderer/SceneTreeRenderer.h"
#include "Renderer/Texture.h"
#include "Renderer/TopLevelAccelerationStructure.h"
#include "Renderer/UniformBuffer.h"
#include "entt/entt.hpp"

class b2World;
class b2Body;
class b2Fixture;

namespace BeeEngine
{
    namespace Editor
    {
        class SceneHierarchyPanel;
        class InspectorPanel;
        class EditorLayer;
    } // namespace Editor
    class Entity;
    class Prefab;
    class Scene
    {
        friend class Entity;
        friend class BeeEngine::Editor::SceneHierarchyPanel;
        friend class BeeEngine::Editor::InspectorPanel;
        friend class BeeEngine::Editor::EditorLayer;
        friend class SceneSerializer;
        friend class PrefabImporter;
        friend class SceneRenderer;

    public:
        struct GPUSceneData
        {
            glm::mat4 view;
            glm::mat4 proj;
            glm::mat4 viewproj;
            glm::vec4 ambientColor;
            glm::vec4 sunlightDirection; // w for sun power
            glm::vec4 sunlightColor;
        };

        struct SceneRendererData
        {
            Ref<UniformBuffer> CameraUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4));
            Ref<BindingSet> CameraBindingSet = BindingSet::Create({{0, *CameraUniformBuffer}});
            Ref<UniformBuffer> MeshSceneDataUniformBuffer = UniformBuffer::Create(sizeof(GPUSceneData));
            Ref<BindingSet> MeshSceneDataBindingSet = BindingSet::Create({{0, *MeshSceneDataUniformBuffer}});
        };

        static Ref<Scene> Copy(Scene& scene);

        Scene();
        void UpdateRuntime();
        void OnViewPortResize(uint32_t width, uint32_t height);

        Entity CreateEntity(const String& name = "Entity");
        void DestroyEntity(Entity entity);
        Entity DuplicateEntity(Entity entity);

        Entity InstantiatePrefab(Prefab& prefab, Entity parent);

        Entity RayCast2D(glm::vec2 start, glm::vec2 end);

        void StartRuntime();
        void StopRuntime();

        Entity GetEntityByUUID(struct UUID uuid);
        Entity GetEntityByName(std::string_view name);
        Entity GetPrimaryCameraEntity();

        [[nodiscard]] bool IsRuntime() const noexcept { return m_IsRuntime; }

        void Clear();

        TopLevelAccelerationStructure& GetTLAS() { return *m_TLAS; }

        SceneRendererData& GetSceneRendererData() { return m_SceneRendererData; }

    private:
        entt::registry m_Registry;

        bool m_IsRuntime = false;
        // void ResetScene();
        b2World* m_2DPhysicsWorld;

        SceneRendererData m_SceneRendererData;

        Ref<TopLevelAccelerationStructure> m_TLAS = TopLevelAccelerationStructure::Create();

        void UpdateScripts();

        std::unordered_map<uint64_t, entt::entity> m_UUIDMap;

        const std::vector<struct NativeScriptInfo>* m_NativeScripts = nullptr;

        void DestroyScripts();

        Entity CreateEntityWithUUID(struct UUID uuid, const String& name);

        void StartPhysicsWorld();

        void StopPhysicsWorld();

        void Update2DPhysics();

        Entity CopyEntity(Entity entity, Scene& targetScene, Entity parent, bool preserveUUID);

        void* CreateRuntimeRigidBody2D(class RigidBody2DComponent& rigidBody,
                                       const class TransformComponent& transform);

        void CreateRuntimeBoxCollider2DFixture(const class TransformComponent& transform,
                                               b2Body* body,
                                               class BoxCollider2DComponent& boxCollider) const;
    };
} // namespace BeeEngine
