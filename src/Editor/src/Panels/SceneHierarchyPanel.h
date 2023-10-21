//
// Created by alexl on 04.06.2023.
//

#pragma once
#include "BeeEngine.h"
#include "Scene/Entity.h"
#include "Locale/Locale.h"
#include <atomic>

namespace BeeEngine::Editor
{
    class SceneHierarchyPanel
    {
    public:
        SceneHierarchyPanel(Locale::Domain& editorDomain)
        : m_EditorDomain(&editorDomain)
        {}
        explicit SceneHierarchyPanel(const Ref<Scene>& context);

        void SetContext(const Ref<Scene>& context);

        void OnGUIRender() noexcept;

        void OnEvent(EventDispatcher& e) noexcept;

        [[nodiscard]] Entity GetSelectedEntity() const noexcept { return m_SelectedEntity; }
        [[nodiscard]] Entity& GetSelectedEntityRef() noexcept { return m_SelectedEntity; }

        void ClearSelection();

    private:
        void DrawEntityNode(Entity entity) noexcept;
        bool OnKeyPressedEvent(KeyPressedEvent* e) noexcept;

        Ref<Scene> m_Context;
        Entity m_SelectedEntity;
        Locale::Domain* m_EditorDomain = nullptr;
    };
}
