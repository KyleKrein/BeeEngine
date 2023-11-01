//
// Created by alexl on 26.05.2023.
//
#include "Debug/Instrumentor.h"
#include "EditorLayer.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Utils/FileDialogs.h"
#include "Core/ResourceManager.h"
#include <../../Engine/Assets/EmbeddedResources.h>
#include "ConfigFile.h"
#include "Scripting/ScriptingEngine.h"
#include "Scripting/MAssembly.h"
#include "Scripting/MClass.h"
#include "Scripting/ScriptGlue.h"
#include "Core/AssetManagement/AssetManager.h"
#include "Core/AssetManagement/EngineAssetRegistry.h"
#include "Core/AssetManagement/AssetRegistrySerializer.h"
#include "FileSystem/File.h"
#include "Locale/LocalizationGenerator.h"

namespace BeeEngine::Editor
{

    void EditorLayer::OnAttach() noexcept
    {
        m_EditorLocaleDomain.SetLocale(Locale::GetSystemLocale());
        auto localizationFilesPaths = Locale::LocalizationGenerator::GetLocalizationFiles(std::filesystem::current_path() / "Localization");
        Locale::LocalizationGenerator::ProcessLocalizationFiles(m_EditorLocaleDomain, localizationFilesPaths);
        m_EditorLocaleDomain.Build();
        ConsoleOutput::SetOutputProvider(&m_Console);
        SetUpMenuBar();
        m_PlayButtonTexture = AssetManager::GetAssetRef<Texture2D>(EngineAssetRegistry::PlayButtonTexture, "en_US");
        m_StopButtonTexture = AssetManager::GetAssetRef<Texture2D>(EngineAssetRegistry::StopButtonTexture, "en_US");

        m_EditorCamera = EditorCamera(45.0f, 1.778f, 0.1f, 1000.0f);
        m_SceneHierarchyPanel.SetContext(m_ViewPort.GetScene());
        m_ContentBrowserPanel.SetContext(m_ViewPort.GetScene());
        m_InspectorPanel.SetContext(m_ViewPort.GetScene());
        m_ActiveScene = m_ViewPort.GetScene();
    }

    void EditorLayer::OnDetach() noexcept
    {
        if(m_ProjectFile)
        {
            ScriptingEngine::Shutdown();
        }
        ConsoleOutput::SetOutputProvider(nullptr);
    }
    void EditorLayer::OnUpdate() noexcept
    {
        BEE_PROFILE_FUNCTION();
        if(m_ProjectFile == nullptr)
            return;
        m_ProjectFile->Update();
        m_EditorCamera.OnUpdate();
        ScriptingEngine::UpdateAllocatorStatistics();
        switch (m_SceneState)
        {
            case SceneState::Edit:
            {
                if(m_ContentBrowserPanel.NeedsToRegenerateSolution())
                    m_ProjectFile->RegenerateSolution();
                if(m_ProjectFile->IsAssemblyReloadPending())
                    ReloadAssembly();
                m_ViewPort.UpdateEditor(m_EditorCamera, m_RenderPhysicsColliders);
                //m_GameBuilder->UpdateAndCompile();
                break;
            }
            case SceneState::Play:
            {
                m_ViewPort.UpdateRuntime(m_RenderPhysicsColliders);
                break;
            }
            case SceneState::Pause:
            {
                break;
            }
            case SceneState::Simulate:
            {
                break;
            }
        }
        if(m_ViewPort.IsNewSceneLoaded())
        {
            if(m_SceneState == SceneState::Play)
            {
                m_SceneState = SceneState::Edit;
                OnSceneStop();
            }
            LoadScene(m_ViewPort.GetScenePath());
        }
        m_FpsCounter.Update();
    }

    void EditorLayer::OnGUIRendering() noexcept
    {
        m_DockSpace.Start();
        if(m_ProjectFile)
        {
            m_MenuBar.Render();
            UIToolbar();
            m_ViewPort.Render(m_EditorCamera);
            m_SceneHierarchyPanel.OnGUIRender();
            m_InspectorPanel.OnGUIRender(m_SceneHierarchyPanel.GetSelectedEntity());
            m_ContentBrowserPanel.OnGUIRender();
            m_AssetPanel.Render();
            m_FpsCounter.Render();
            m_Console.RenderGUI();
            m_LocalizationPanel->Render();
            ImGui::Begin("Settings");
            ImGui::Checkbox("Render physics colliders", &m_RenderPhysicsColliders);
            ImGui::End();
        }
        else
        {
            ImGui::Begin("Project");
            if(ImGui::Button("Load project"))
            {
                auto projectPath = FileDialogs::OpenFile({"BeeEngine Project", "*.beeproj"});
                if(projectPath.IsEmpty())
                {
                    BeeCoreError("Unable to open file");
                    goto newProject;
                }
                String pathString = projectPath.AsUTF8();
                auto name = projectPath.GetFileNameWithoutExtension().AsUTF8();
                pathString = projectPath.RemoveFileName().AsUTF8();
                m_ProjectFile = CreateScope<ProjectFile>(pathString, name, &m_EditorAssetManager);
                m_ContentBrowserPanel.SetWorkingDirectory(m_ProjectFile->GetProjectPath());
                m_ViewPort.SetWorkingDirectory(m_ProjectFile->GetProjectPath());
                m_ViewPort.SetDomain(&m_ProjectFile->GetProjectLocaleDomain());
                m_InspectorPanel.SetWorkingDirectory(m_ProjectFile->GetProjectPath());
                ResourceManager::ProjectName = m_ProjectFile->GetProjectName();

                SetupGameLibrary();

                if(File::Exists(m_ProjectFile->GetProjectAssetRegistryPath()))
                {
                    AssetRegistrySerializer assetRegistrySerializer(&m_EditorAssetManager, m_ProjectFile->GetProjectPath(), m_ProjectFile->GetAssetRegistryID());
                    assetRegistrySerializer.Deserialize(m_ProjectFile->GetProjectAssetRegistryPath());
                }

                m_InspectorPanel.SetProjectAssetRegistryID(m_ProjectFile->GetAssetRegistryID());
                m_InspectorPanel.SetProject(m_ProjectFile.get());
                m_AssetPanel.SetProject(m_ProjectFile.get());
                m_ContentBrowserPanel.SetProject(m_ProjectFile.get());
                m_AssetPanel.SetAssetDeletedCallback([this](AssetHandle handle){
                    DeleteAsset(handle);
                });
                m_LocalizationPanel = CreateScope<Locale::ImGuiLocalizationPanel>(m_ProjectFile->GetProjectLocaleDomain(), m_ProjectFile->GetProjectPath());
                auto scenePath = m_ProjectFile->GetLastUsedScenePath();
                if(!scenePath.IsEmpty())
                {
                    LoadScene(scenePath);
                }
            }
            newProject:
            if(ImGui::Button("New project"))
            {
                auto projectPath = FileDialogs::OpenFolder(/*{"BeeEngine Project", "*.beeproj"}*/);
                if(projectPath.IsEmpty())
                {
                    BeeCoreError("Unable to open folder");
                    goto end;
                }
                auto name = projectPath.GetFileName().AsUTF8();
                m_ProjectFile = CreateScope<ProjectFile>(projectPath, name, &m_EditorAssetManager);
                m_ContentBrowserPanel.SetWorkingDirectory(m_ProjectFile->GetProjectPath());
                m_ViewPort.SetWorkingDirectory(m_ProjectFile->GetProjectPath());
                m_ViewPort.SetDomain(&m_ProjectFile->GetProjectLocaleDomain());
                m_InspectorPanel.SetWorkingDirectory(m_ProjectFile->GetProjectPath());
                ResourceManager::ProjectName = m_ProjectFile->GetProjectName();

                m_InspectorPanel.SetProjectAssetRegistryID(m_ProjectFile->GetAssetRegistryID());
                m_InspectorPanel.SetProject(m_ProjectFile.get());
                m_ContentBrowserPanel.SetProject(m_ProjectFile.get());

                m_AssetPanel.SetProject(m_ProjectFile.get());
                m_AssetPanel.SetAssetDeletedCallback([this](AssetHandle handle){
                    DeleteAsset(handle);
                });

                SetupGameLibrary();
                m_LocalizationPanel = CreateScope<Locale::ImGuiLocalizationPanel>(m_ProjectFile->GetProjectLocaleDomain(), m_ProjectFile->GetProjectPath());
            }
            end:
            ImGui::End();
        }
        m_DockSpace.End();
    }

    void EditorLayer::OnEvent(EventDispatcher &event) noexcept
    {
        if(m_ProjectFile == nullptr)
            return;
        if(m_SceneState == SceneState::Play)
            return;
        m_EditorCamera.OnEvent(event);
        m_ViewPort.OnEvent(event);
        m_SceneHierarchyPanel.OnEvent(event);
        DISPATCH_EVENT(event, KeyPressedEvent, EventType::KeyPressed, OnKeyPressed);
    }

    void EditorLayer::SetUpMenuBar()
    {
        MenuBarElement fileMenu = {m_EditorLocaleDomain.Translate("menubar.file")};
        fileMenu.AddChild({m_EditorLocaleDomain.Translate("menubar.file.newScene"), [this](){
            m_SceneHierarchyPanel.ClearSelection();
            m_ViewPort.GetScene()->Clear();
            m_ScenePath.Clear();
        }});
        fileMenu.AddChild({m_EditorLocaleDomain.Translate("menubar.file.openScene"), [this](){
            auto filepath = BeeEngine::FileDialogs::OpenFile({"BeeEngine Scene", "*.beescene"});
            if(filepath.IsEmpty())
            {
                BeeCoreError("Unable to open file");
                return;
            }
            LoadScene(filepath);
        }});
        fileMenu.AddChild({m_EditorLocaleDomain.Translate("menubar.file.saveScene"), [this](){
            SaveScene();
        }});
        fileMenu.AddChild({m_EditorLocaleDomain.Translate("menubar.file.saveSceneAs"), [this](){
            auto filepath = BeeEngine::FileDialogs::SaveFile({"BeeEngine Scene", "*.beescene"});
            if(filepath.IsEmpty())
            {
                BeeCoreError("Unable to save to file");
                return;
            }
            if(filepath.GetExtension() != ".beescene")
                filepath.ReplaceExtension(".beescene");
            m_ScenePath = filepath;
            SceneSerializer serializer(m_ActiveScene);
            serializer.Serialize(m_ScenePath);
        }});
        fileMenu.AddChild({m_EditorLocaleDomain.Translate("menubar.file.exit"), [](){BeeEngine::Application::GetInstance().Close();}});
        m_MenuBar.AddElement(fileMenu);

        MenuBarElement BuildMenu = {m_EditorLocaleDomain.Translate("menubar.build")};
        BuildMenu.AddChild({m_EditorLocaleDomain.Translate("menubar.build.regenerateVSSolution"), [this](){
            if(!m_ProjectFile)
            {
                BeeCoreError("No project loaded");
                return;
            }
            m_ProjectFile->RegenerateSolution();
        }});
        BuildMenu.AddChild({m_EditorLocaleDomain.Translate("menubar.build.reloadScripts"), [this](){
            ReloadAssembly();
        }});
        m_MenuBar.AddElement(BuildMenu);

        MenuBarElement ViewMenu = {m_EditorLocaleDomain.Translate("menubar.view")};
        ViewMenu.AddChild({m_EditorLocaleDomain.Translate("menubar.view.outputConsole"), [this](){
            m_Console.Toggle();
        }});
        ViewMenu.AddChild({m_EditorLocaleDomain.Translate("menubar.view.localization"), [this](){
            m_LocalizationPanel->SwitchOpened();
        }});
        m_MenuBar.AddElement(ViewMenu);
    }

    void EditorLayer::ReloadAssembly()
    {
        const Path tempPath = m_ProjectFile->GetProjectPath() / ".beeengine" / "temp.beescene";
        SceneSerializer serializer(m_ActiveScene);
        serializer.Serialize(tempPath);

        ScriptingEngine::ReloadAssemblies();
        m_SceneHierarchyPanel.ClearSelection();
        m_ViewPort.GetScene()->Clear();
        serializer.Deserialize(tempPath);
        m_ViewPort.GetScene()->OnViewPortResize(m_ViewPort.GetWidth(), m_ViewPort.GetHeight());
    }

    void EditorLayer::SetupGameLibrary()
    {
        /*if(!m_GameBuilder)
        {
            m_GameBuilder = CreateScope<GameBuilder>(m_ProjectFile->GetProjectPath(), ConfigFile::LoadCompilerConfiguration());
        }*/
        //ScriptingEngine::EnableDebugging(); //TODO: enable only in debug builds
        ScriptingEngine::Init();
        ScriptingEngine::LoadCoreAssembly("libs/BeeEngine.Core.dll");
        ScriptingEngine::LoadGameAssembly(m_ProjectFile->GetProjectPath() / ".beeengine" / "build"/ "GameLibrary.dll");
        ScriptGlue::Register();
        //auto& gameAssembly = ScriptingEngine::LoadGameAssembly(m_ProjectFile->GetProjectPath() / ".beeengine" / "GameLibrary.dll");
        /*if(m_GameLibrary)
        {
            m_GameLibrary->Reload();
            m_NativeScriptFactory->Reload();
        }
        else
        {
            std::string libraryName = "GameLibrary";
            m_GameLibrary = CreateScope<DynamicLibrary>(m_ProjectFile->GetProjectPath(), libraryName);
            m_NativeScriptFactory = CreateScope<NativeScriptFactory>(m_NativeScriptData);
        }
        if(m_GameLibrary->IsLoaded())
        {
            InitFunction = reinterpret_cast<decltype(InitFunction)>(m_GameLibrary->GetFunction("InitGameLibrary"));
            (*InitFunction)(&m_NativeScriptData);
        }
        m_NativeScripts = m_NativeScriptFactory->GetNativeScripts();*/
    }

    void EditorLayer::UIToolbar() noexcept
    {
        auto& colors = ImGui::GetStyle().Colors;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              {buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f});
        const auto& buttonActive = colors[ImGuiCol_ButtonActive];
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              {buttonActive.x, buttonActive.y, buttonActive.z, 0.5f});
        ImGui::Begin("##Toolbar", nullptr, ImGuiWindowFlags_NoDecoration| ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
        ImTextureID textureID = (ImTextureID)(m_SceneState == SceneState::Edit ? m_PlayButtonTexture->GetRendererID() : m_StopButtonTexture->GetRendererID());
        float size = ImGui::GetWindowHeight() - 4;
        ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x * 0.5f) - (size * 0.5f));
        if(ImGui::ImageButton(textureID, ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
        {
            if(m_SceneState == SceneState::Edit)
            {
                m_SceneState = SceneState::Play;
            }
            else
            {
                m_SceneState = SceneState::Edit;
            }
            switch (m_SceneState)
            {
                case SceneState::Edit:
                {
                    OnSceneStop();
                    break;
                }
                case SceneState::Play:
                {
                    OnScenePlay();
                    break;
                }
                case SceneState::Pause:
                {
                    OnScenePause();
                    break;
                }
                case SceneState::Simulate:
                {
                    OnSceneSimulate();
                    break;
                }
            }
        }
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);
        ImGui::End();
    }

    void EditorLayer::OnScenePlay() noexcept
    {
        m_EditorScene = Scene::Copy(*m_ActiveScene);
        m_ActiveScene->StartRuntime();
    }

    void EditorLayer::OnScenePause() noexcept
    {

    }

    void EditorLayer::OnSceneStop() noexcept
    {
        m_ViewPort.GetScene()->StopRuntime();
        m_ActiveScene.reset();
        m_ActiveScene = m_EditorScene;
        SetScene(m_ActiveScene);
        m_ActiveScene->OnViewPortResize(m_ViewPort.GetWidth(), m_ViewPort.GetHeight());
    }

    void EditorLayer::OnSceneSimulate() noexcept
    {

    }

    void EditorLayer::LoadScene(const Path& path)
    {
        m_SceneHierarchyPanel.ClearSelection();
        m_ViewPort.GetScene()->Clear();
        m_ScenePath = path;
        SceneSerializer serializer(m_ActiveScene);
        serializer.Deserialize(m_ScenePath);
        m_ViewPort.GetScene()->OnViewPortResize(m_ViewPort.GetWidth(), m_ViewPort.GetHeight());
        m_ProjectFile->SetLastUsedScenePath(m_ScenePath);
    }

    void EditorLayer::SetScene(const Ref<Scene> &sharedPtr)
    {
        m_SceneHierarchyPanel.ClearSelection();
        m_ViewPort.SetScene(sharedPtr);
        m_SceneHierarchyPanel.SetContext(sharedPtr);
        m_ContentBrowserPanel.SetContext(sharedPtr);
        m_InspectorPanel.SetContext(sharedPtr);
    }

    void EditorLayer::SaveScene()
    {
        SaveAssetRegistry();
        if(m_ScenePath.IsEmpty())
        {
            auto filepath = BeeEngine::FileDialogs::SaveFile({"BeeEngine Scene", "*.beescene"});
            if(filepath.IsEmpty())
            {
                BeeCoreError("Unable to save to file");
                return;
            }
            if(filepath.GetExtension()!= ".beescene")
                filepath.ReplaceExtension(".beescene");
            m_ScenePath = filepath;
        }
        SceneSerializer serializer(m_ActiveScene);
        serializer.Serialize(m_ScenePath);
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent *e) noexcept
    {
        bool ctrl = Input::KeyPressed(Key::LeftControl) || Input::KeyPressed(Key::RightControl);
        if(ctrl && Input::KeyPressed(Key::S))
        {
            SaveScene();
            return true;
        }
        return false;
    }

    void EditorLayer::SaveAssetRegistry()
    {
        AssetRegistrySerializer serializer(&m_EditorAssetManager, m_ProjectFile->GetProjectPath(), m_ProjectFile->GetAssetRegistryID());
        serializer.Serialize(m_ProjectFile->GetProjectAssetRegistryPath());
    }

    void EditorLayer::DeleteAsset(const AssetHandle &handle)
    {
        auto type = m_EditorAssetManager.GetAsset(handle)->GetType();
        if(type == AssetType::Texture2D)
        {
            auto view = m_ActiveScene->m_Registry.view<SpriteRendererComponent>();
            for(auto& entity : view)
            {
                auto& spriteRenderer = view.get<SpriteRendererComponent>(entity);
                if(spriteRenderer.HasTexture && spriteRenderer.TextureHandle == handle)
                {
                    spriteRenderer.HasTexture = false;
                }
            }
        }
        else if(type == AssetType::Font)
        {
            auto view = m_ActiveScene->m_Registry.view<TextRendererComponent>();
            for(auto& entity : view)
            {
                auto& textComponent = view.get<TextRendererComponent>(entity);
                if(textComponent.FontHandle == handle)
                {
                    textComponent.FontHandle = EngineAssetRegistry::OpenSansRegular;
                }
            }
        }
        m_EditorAssetManager.RemoveAsset(handle);
        SaveAssetRegistry();
    }
}
