//
// Created by alexl on 02.06.2023.
//

#pragma once

#include "entt/entt.hpp"
#include "EntityID.h"
#include "Renderer/EditorCamera.h"
#include "Renderer/UniformBuffer.h"
#include "Renderer/Model.h"
#include "Renderer/Texture.h"

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
        Scene();
        void UpdateRuntime();
        void UpdateEditor(EditorCamera& camera);
        void OnViewPortResize(uint32_t width, uint32_t height);

        Entity CreateEntity(std::string_view name = "Entity");
        void DestroyEntity(Entity entity);

        void StartRuntime();
        void StopRuntime();

        void Clear();
    private:
        entt::registry m_Registry;

        bool m_IsRuntime = false;
        //void ResetScene();


        void UpdateScripts();

        Model* m_RectModel = nullptr;
        Texture2D* m_BlankTexture = nullptr;
        Ref<BindingSet> m_CameraBindingSet;
        Ref<BindingSet> m_TextureBindingSet;
        Ref<UniformBuffer> m_CameraUniformBuffer;
    };
}
