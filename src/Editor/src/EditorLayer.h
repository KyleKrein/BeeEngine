//
// Created by alexl on 26.05.2023.
//

#pragma once

#include "BeeEngine.h"
#include "Panels/ViewPort.h"
#include "Panels/DockSpace.h"
#include "Gui/ImGui/FpsCounter.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/MenuBar.h"
#include "Scene/SceneSerializer.h"

namespace BeeEngine::Editor
{
    class EditorLayer: public Layer
    {
    public:
        ~EditorLayer() noexcept override = default;

        void OnAttach() noexcept override;
        void OnDetach() noexcept override;
        void OnUpdate() noexcept override;
        void OnGUIRendering() noexcept override;
        void OnEvent(EventDispatcher& event) noexcept override;
    private:
        DockSpace m_DockSpace {};
        MenuBar m_MenuBar {};
        ViewPort m_ViewPort {100, 100};
        SceneSerializer m_SceneSerializer {m_ViewPort.GetScene()};
        std::string m_ScenePath;
        BeeEngine::Internal::FpsCounter m_FpsCounter {};
        SceneHierarchyPanel m_SceneHierarchyPanel {};
        InspectorPanel m_InspectorPanel {};

        void SetUpMenuBar();
    };
}