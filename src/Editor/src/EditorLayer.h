//
// Created by alexl on 26.05.2023.
//

#pragma once

#include "BeeEngine.h"
#include "ConfigFile.h"
#include "Core/AssetManagement/Asset.h"
#include "Core/AssetManagement/EditorAssetManager.h"
#include "Gui/ImGui/FpsCounter.h"
#include "Gui/ImGui/ImGuiOutputConsole.h"
#include "ImGuiNativeDragAndDrop.h"
#include "Locale/ImGuiLocalizationPanel.h"
#include "Locale/Locale.h"
#include "Panels/AssetPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/DockSpace.h"
#include "Panels/ImGuiUIEditor.h"
#include "Panels/InspectorPanel.h"
#include "Panels/MenuBar.h"
#include "Panels/ProjectSettings.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ViewPort.h"
#include "ProjectFile.h"
#include "kdbindings/property.h"
#include <functional>

namespace BeeEngine::Editor
{
    class EditorLayer : public Layer
    {
    public:
        explicit EditorLayer(ConfigFile& config) noexcept : m_Config(config) {}
        ~EditorLayer() noexcept override = default;
        Property<Scope<ProjectFile>> Project{nullptr};

        void OnAttach() noexcept override;
        void OnDetach() noexcept override;
        void OnUpdate(FrameData& frameData) noexcept override;
        void OnGUIRendering() noexcept override;
        void OnEvent(EventDispatcher& event) noexcept override;

    private:
        Jobs::SpinLock m_BigLock{};
        ImGuiNativeDragAndDrop m_DragAndDrop{};
        Locale::Domain m_EditorLocaleDomain{"Editor"};
        ImGuiOutputConsole m_Console{};
        EditorAssetManager m_EditorAssetManager{};
        EditorCamera m_EditorCamera = {};
        DockSpace m_DockSpace{};
        MenuBar m_MenuBar{};
        ConfigFile& m_Config;
        AssetPanel m_AssetPanel{Project, &m_EditorAssetManager, m_EditorLocaleDomain, m_Config};
        ContentBrowserPanel m_ContentBrowserPanel{
            Project, std::filesystem::current_path(), m_EditorLocaleDomain, m_Config};
        SceneHierarchyPanel m_SceneHierarchyPanel{m_EditorLocaleDomain};
        ViewPort m_ViewPort{
            Project, 100, 100, m_SceneHierarchyPanel.GetSelectedEntityRef(), Color4::White, m_EditorAssetManager};
        BeeEngine::Internal::FpsCounter m_FpsCounter{};
        InspectorPanel m_InspectorPanel{Project, &m_EditorAssetManager, m_EditorLocaleDomain};
        Scope<Locale::ImGuiLocalizationPanel> m_LocalizationPanel = nullptr;
        Scope<ProjectSettings> m_ProjectSettings = nullptr;

        bool m_RenderPhysicsColliders = false;

        Ref<Scene> m_ActiveScene = nullptr;
        Ref<Scene> m_EditorScene = nullptr;

        // Scope<DynamicLibrary> m_GameLibrary = nullptr;
        // Scope<GameBuilder> m_GameBuilder = nullptr;
        // Scope<NativeScriptFactory> m_NativeScriptFactory = nullptr;
        // BeeEngineNativeScriptRegistryData m_NativeScriptData = {};
        // void*(*InitFunction)(void*) = nullptr;

        Ref<Texture2D> m_PlayButtonTexture = nullptr;
        Ref<Texture2D> m_StopButtonTexture = nullptr;

        enum class SceneState
        {
            Edit = 0,
            Play = 1,
            Pause = 2,
            Simulate = 3
        };
        SceneState m_SceneState = SceneState::Edit;
        bool m_ShowBuildProjectPopup = false;
        ImGuiUIEditor m_UIEditor{};
        bool m_ShowEditorSettings = false;

        std::function<void()> m_SaveSceneAsFunc;

        // std::vector<NativeScriptInfo> m_NativeScripts = {};

        void SetUpMenuBar();

        void UIToolbar() noexcept;

        void SetupGameLibrary();

        void LoadScene(const AssetHandle& handle);

        bool OnKeyPressed(KeyPressedEvent* e) noexcept;

        void OnScenePlay() noexcept;
        void OnScenePause() noexcept;
        void OnSceneStop() noexcept;
        void OnSceneSimulate() noexcept;

        void RenderChooseProjectScreen();

        void RenderEditor();

        void RenderEditorSettings(bool& show);

        void ReloadAssembly();

        void SaveSceneAs();

        void SaveScene();

        void SaveAssetRegistry();

        void DeleteAsset(const AssetHandle& handle);

        String GenerateImGuiINIFile() const;

        void SetDefaultImGuiWindowLayoutIfNotPresent();

        void DrawBuildProjectPopup();

        void SetImGuiFontSize(float size);
    };
} // namespace BeeEngine::Editor
