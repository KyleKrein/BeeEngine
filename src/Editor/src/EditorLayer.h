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
#include "Panels/ContentBrowserPanel.h"
#include "ProjectFile.h"
#include "Utils/DynamicLibrary.h"
#include "Scene/NativeScriptFactory.h"
#include "NativeScripting/GameBuilder.h"
#include "Core/AssetManagement/EditorAssetManager.h"
#include "Panels/AssetPanel.h"
#include "Core/Logging/ConsoleOutput.h"
#include "Panels/ImGuiOutputConsole.h"
#include "Locale/Locale.h"
#include "Locale/ImGuiLocalizationPanel.h"
#include "ImGuiNativeDragAndDrop.h"
#include "JobSystem/AdaptiveMutex.h"

namespace BeeEngine::Editor
{
    class EditorLayer: public Layer
    {
    public:
        ~EditorLayer() noexcept override = default;

        void OnAttach() noexcept override;
        void OnDetach() noexcept override;
        void OnUpdate(FrameData& frameData) noexcept override;
        void OnGUIRendering() noexcept override;
        void OnEvent(EventDispatcher& event) noexcept override;
    private:
        Jobs::SpinLock m_BigLock {};
        ImGuiNativeDragAndDrop m_DragAndDrop {};
        Locale::Domain m_EditorLocaleDomain {"Editor"};
        ImGuiOutputConsole m_Console {};
        EditorAssetManager m_EditorAssetManager {};
        EditorCamera m_EditorCamera = {};
        DockSpace m_DockSpace {};
        MenuBar m_MenuBar {};
        AssetPanel m_AssetPanel {&m_EditorAssetManager, m_EditorLocaleDomain};
        ContentBrowserPanel m_ContentBrowserPanel {std::filesystem::current_path(), m_EditorLocaleDomain};
        SceneHierarchyPanel m_SceneHierarchyPanel {m_EditorLocaleDomain};
        ViewPort m_ViewPort {100, 100, m_SceneHierarchyPanel.GetSelectedEntityRef()};
        Path m_ScenePath;
        BeeEngine::Internal::FpsCounter m_FpsCounter {};
        InspectorPanel m_InspectorPanel {&m_EditorAssetManager, m_EditorLocaleDomain};
        Scope<ProjectFile> m_ProjectFile = nullptr;
        Scope<Locale::ImGuiLocalizationPanel> m_LocalizationPanel = nullptr;

        bool m_RenderPhysicsColliders = false;

        Ref<Scene> m_ActiveScene = nullptr;
        Ref<Scene> m_EditorScene = nullptr;

        //Scope<DynamicLibrary> m_GameLibrary = nullptr;
        //Scope<GameBuilder> m_GameBuilder = nullptr;
        //Scope<NativeScriptFactory> m_NativeScriptFactory = nullptr;
        //BeeEngineNativeScriptRegistryData m_NativeScriptData = {};
        //void*(*InitFunction)(void*) = nullptr;

        Ref<Texture2D> m_PlayButtonTexture = nullptr;
        Ref<Texture2D> m_StopButtonTexture = nullptr;

        //std::vector<NativeScriptInfo> m_NativeScripts = {};

        void SetUpMenuBar();

        void UIToolbar() noexcept;

        void SetupGameLibrary();

        void LoadScene(const Path& path);

        bool OnKeyPressed(KeyPressedEvent* e) noexcept;

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

        void ReloadAssembly();

        void SetScene(const Ref <Scene> &sharedPtr);

        void SaveScene();

        void SaveAssetRegistry();

        void DeleteAsset(const AssetHandle &handle);
    };
}
