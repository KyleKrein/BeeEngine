//
// Created by alexl on 02.06.2023.
//

#pragma once

#include "entt/entt.hpp"
#include "Renderer/EditorCamera.h"
#include "Renderer/UniformBuffer.h"
#include "Renderer/Model.h"
#include "Renderer/Texture.h"

class b2World;

namespace BeeEngine
{
    namespace Editor
    {
        class SceneHierarchyPanel;
        class InspectorPanel;
    }
    class Entity;
    class Scene
    {
        friend class Entity;
        friend class BeeEngine::Editor::SceneHierarchyPanel;
        friend class BeeEngine::Editor::InspectorPanel;
        friend class SceneSerializer;
    public:
        static Ref<Scene> Copy(const Scene& scene);


        Scene();
        void UpdateRuntime();
        void UpdateEditor(EditorCamera& camera);
        void OnViewPortResize(uint32_t width, uint32_t height);

        Entity CreateEntity(const std::string& name = "Entity");
        void DestroyEntity(Entity entity);
        Entity DuplicateEntity(Entity entity);

        void StartRuntime();
        void StopRuntime();

        Entity GetEntityByUUID(struct UUID uuid);
        Entity GetEntityByName(std::string_view name);

        [[nodiscard]] bool IsRuntime() const noexcept { return m_IsRuntime; }

        void Clear();
    private:
        entt::registry m_Registry;

        bool m_IsRuntime = false;
        //void ResetScene();
        b2World* m_2DPhysicsWorld;

        void UpdateScripts();

        Model* m_RectModel = nullptr;
        Model* m_CircleModel = nullptr;
        Texture2D* m_BlankTexture = nullptr;
        Ref<BindingSet> m_CameraBindingSet;
        Ref<UniformBuffer> m_CameraUniformBuffer;

        std::unordered_map<uint64_t , entt::entity> m_UUIDMap;

        const std::vector<struct NativeScriptInfo>* m_NativeScripts = nullptr;

        void DestroyScripts();

        Entity CreateEntityWithUUID(struct UUID uuid, const std::string& name);

        void StartPhysicsWorld();

        void StopPhysicsWorld();

        void Update2DPhysics();
    };
}
