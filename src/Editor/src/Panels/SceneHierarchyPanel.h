//
// Created by alexl on 04.06.2023.
//

#pragma once
#include "BeeEngine.h"
#include "Scene/Entity.h"

namespace BeeEngine::Editor
{
    class SceneHierarchyPanel
    {
    public:
        SceneHierarchyPanel() = default;
        explicit SceneHierarchyPanel(const Ref<Scene>& context);

        void SetContext(const Ref<Scene>& context);

        void OnGUIRender() noexcept;

        [[nodiscard]] Entity GetSelectedEntity() const noexcept { return m_SelectedEntity; }

    private:
        void DrawEntityNode(Entity entity) noexcept;

        Ref<Scene> m_Context;
        Entity m_SelectedEntity;
    };
}
