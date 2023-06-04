//
// Created by alexl on 02.06.2023.
//

#pragma once

#include "entt/entt.hpp"
#include "EntityID.h"

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
    public:
        void OnUpdate();
        void OnViewPortResize(uint32_t width, uint32_t height);

        Entity CreateEntity(std::string_view name = "Entity");
        void DestroyEntity(Entity entity);
    private:
        entt::registry m_Registry;

        void UpdateScripts();
    };
}
