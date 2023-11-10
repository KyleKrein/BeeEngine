//
// Created by alexl on 02.06.2023.
//

#pragma once

#include "entt/entt.hpp"
#include "Renderer/EditorCamera.h"
#include "Renderer/UniformBuffer.h"
#include "Renderer/Model.h"
#include "Renderer/Texture.h"
#include "Renderer/SceneTreeRenderer.h"

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
    }
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
        static Ref<Scene> Copy(Scene& scene);


        Scene();
        void UpdateRuntime();
        void OnViewPortResize(uint32_t width, uint32_t height);

        Entity CreateEntity(const std::string& name = "Entity");
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
    private:
        entt::registry m_Registry;

        bool m_IsRuntime = false;
        //void ResetScene();
        b2World* m_2DPhysicsWorld;

        void UpdateScripts();

        std::unordered_map<uint64_t , entt::entity> m_UUIDMap;

        const std::vector<struct NativeScriptInfo>* m_NativeScripts = nullptr;

        void DestroyScripts();

        Entity CreateEntityWithUUID(struct UUID uuid, const std::string& name);

        void StartPhysicsWorld();

        void StopPhysicsWorld();

        void Update2DPhysics();

        Entity CopyEntity(Entity entity, Scene& targetScene, Entity parent, bool preserveUUID);

        void *
        CreateRuntimeRigidBody2D(class RigidBody2DComponent &rigidBody,
                                 const class TransformComponent &transform);

        void CreateRuntimeBoxCollider2DFixture(const class TransformComponent &transform, b2Body *body,
                                               class BoxCollider2DComponent &boxCollider) const;
    };
}
