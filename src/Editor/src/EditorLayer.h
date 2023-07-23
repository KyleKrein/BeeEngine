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
#include "Panels/AssetPanel.h"
#include "ProjectFile.h"
#include "Utils/DynamicLibrary.h"
#include "Scene/NativeScriptFactory.h"

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
        EditorCamera m_EditorCamera = {};
        DockSpace m_DockSpace {};
        MenuBar m_MenuBar {};
        AssetPanel m_AssetPanel {std::filesystem::current_path()};
        SceneHierarchyPanel m_SceneHierarchyPanel {};
        ViewPort m_ViewPort {100, 100, m_SceneHierarchyPanel.GetSelectedEntityRef()};
        SceneSerializer m_SceneSerializer {m_ViewPort.GetScene()};
        std::filesystem::path m_ScenePath;
        BeeEngine::Internal::FpsCounter m_FpsCounter {};
        InspectorPanel m_InspectorPanel {};
        Scope<ProjectFile> m_ProjectFile = nullptr;

        Scope<DynamicLibrary> m_GameLibrary = nullptr;
        Scope<NativeScriptFactory> m_NativeScriptFactory = nullptr;
        BeeEngineNativeScriptRegistryData m_NativeScriptData = {};
        void*(*InitFunction)(void*) = nullptr;

        Ref<Texture2D> m_PlayButtonTexture = nullptr;
        Ref<Texture2D> m_StopButtonTexture = nullptr;

        std::vector<NativeScriptInfo> m_NativeScripts = {};

        void SetUpMenuBar();

        void UIToolbar() noexcept;

        void SetupGameLibrary();

        void LoadScene(const std::filesystem::path& path);

        enum class SceneState
        {
            Edit = 0,
            Play = 1,
            Pause = 2,
            Simulate = 3
        };
        SceneState m_SceneState = SceneState::Edit;

        void OnScenePlay() noexcept;
        void OnScenePause() noexcept;
        void OnSceneStop() noexcept;
        void OnSceneSimulate() noexcept;
    };
}
