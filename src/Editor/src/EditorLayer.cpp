//
// Created by alexl on 26.05.2023.
//

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

namespace BeeEngine::Editor
{

    void EditorLayer::OnAttach() noexcept
    {
        SetUpMenuBar();

        m_PlayButtonTexture = Texture2D::CreateFromMemory(Internal::GetEmbeddedResource(EmbeddedResource::PlayButtonTexture));
        m_StopButtonTexture = Texture2D::CreateFromMemory(Internal::GetEmbeddedResource(EmbeddedResource::StopButtonTexture));

        m_EditorCamera = EditorCamera(45.0f, 1.778f, 0.1f, 1000.0f);
        m_SceneHierarchyPanel.SetContext(m_ViewPort.GetScene());
        m_InspectorPanel.SetContext(m_ViewPort.GetScene());
        m_ActiveScene = m_ViewPort.GetScene();
    }

    void EditorLayer::OnDetach() noexcept
    {
        if(m_ProjectFile)
        {
            ScriptingEngine::Shutdown();
        }
    }
    void EditorLayer::OnUpdate() noexcept
    {
        if(m_ProjectFile == nullptr)
            return;
        m_ProjectFile->Update();
        m_EditorCamera.OnUpdate();
        ScriptingEngine::UpdateAllocatorStatistics();
        switch (m_SceneState)
        {
            case SceneState::Edit:
            {
                if(m_AssetPanel.NeedsToRegenerateSolution())
                    m_ProjectFile->RegenerateSolution();
                if(m_ProjectFile->IsAssemblyReloadPending())
                    ReloadAssembly();
                m_ViewPort.UpdateEditor(m_EditorCamera);
                //m_GameBuilder->UpdateAndCompile();
                break;
            }
            case SceneState::Play:
            {
                m_ViewPort.UpdateRuntime();
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
            m_AssetPanel.OnGUIRender();
            m_FpsCounter.Render();
        }
        else
        {
            ImGui::Begin("Project");
            if(ImGui::Button("Load project"))
            {
                auto path = FileDialogs::OpenFile({"BeeEngine Project", "*.beeproj"});
                if(path.empty())
                {
                    BeeCoreError("Unable to open file");
                    goto newProject;
                }
                std::filesystem::path projectPath = (path);
                auto name = ResourceManager::GetNameFromFilePath(projectPath.string());
                path = projectPath.remove_filename().string();
                path.pop_back();
                m_ProjectFile = CreateScope<ProjectFile>(path, name);
                m_AssetPanel.SetWorkingDirectory(m_ProjectFile->GetProjectPath());
                m_ViewPort.SetWorkingDirectory(m_ProjectFile->GetProjectPath());
                m_InspectorPanel.SetWorkingDirectory(m_ProjectFile->GetProjectPath());
                ResourceManager::ProjectName = m_ProjectFile->GetProjectName();

                SetupGameLibrary();

                auto scenePath = m_ProjectFile->GetLastUsedScenePath();
                if(!scenePath.empty())
                {
                    LoadScene(scenePath);
                }
            }
            newProject:
            if(ImGui::Button("New project"))
            {
                auto path = FileDialogs::SaveFile({"BeeEngine Project", "*.beeproj"});
                if(path.empty())
                {
                    BeeCoreError("Unable to open file");
                    goto end;
                }
                if(!path.ends_with(".beeproj"))
                {
                    path += ".beeproj";
                }
                std::filesystem::path projectPath = path;
                auto name = ResourceManager::GetNameFromFilePath(projectPath.string());
                path = projectPath.remove_filename().string();
                path.pop_back();
                m_ProjectFile = CreateScope<ProjectFile>(path, name);
                m_AssetPanel.SetWorkingDirectory(m_ProjectFile->GetProjectPath());
                m_ViewPort.SetWorkingDirectory(m_ProjectFile->GetProjectPath());
                m_InspectorPanel.SetWorkingDirectory(m_ProjectFile->GetProjectPath());
                ResourceManager::ProjectName = m_ProjectFile->GetProjectName();

                SetupGameLibrary();
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
        MenuBarElement fileMenu = {"File"};
        fileMenu.AddChild({"New Scene", [this](){
            m_SceneHierarchyPanel.ClearSelection();
            m_ViewPort.GetScene()->Clear();
            m_ScenePath.clear();
        }});
        fileMenu.AddChild({"Open Scene", [this](){
            auto filepath = BeeEngine::FileDialogs::OpenFile({"BeeEngine Scene", "*.beescene"});
            if(filepath.empty())
            {
                BeeCoreError("Unable to open file");
                return;
            }
            LoadScene(filepath);
        }});
        fileMenu.AddChild({"Save Scene", [this](){
            SaveScene();
        }});
        fileMenu.AddChild({"Save Scene As...", [this](){
            auto filepath = BeeEngine::FileDialogs::SaveFile({"BeeEngine Scene", "*.beescene"});
            if(filepath.empty())
            {
                BeeCoreError("Unable to save to file");
                return;
            }
            if(!filepath.ends_with(".beescene"))
                filepath += ".beescene";
            m_ScenePath = filepath;
            SceneSerializer serializer(m_ActiveScene);
            serializer.Serialize(m_ScenePath);
        }});
        fileMenu.AddChild({"Exit", [](){BeeEngine::Application::GetInstance().Close();}});
        m_MenuBar.AddElement(fileMenu);

        MenuBarElement BuildMenu = {"Build"};
        BuildMenu.AddChild({"Regenerate VS Solution", [this](){
            if(!m_ProjectFile)
            {
                BeeCoreError("No project loaded");
                return;
            }
            m_ProjectFile->RegenerateSolution();
        }});
        BuildMenu.AddChild({"Reload Scripts", [this](){
            ReloadAssembly();
        }});
        m_MenuBar.AddElement(BuildMenu);
    }

    void EditorLayer::ReloadAssembly()
    {
        const std::filesystem::path tempPath = m_ProjectFile->GetProjectPath() / ".beeengine" / "temp.beescene";
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
        ScriptingEngine::EnableDebugging();
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
        if(m_ScenePath.empty())
        {
            auto filepath = BeeEngine::FileDialogs::SaveFile({"BeeEngine Scene", "*.beescene"});
            if(filepath.empty())
            {
                BeeCoreError("Unable to save to file");
                return;
            }
            if(!filepath.ends_with(".beescene"))
                filepath += ".beescene";
            m_ScenePath = filepath;
        }
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

    void EditorLayer::LoadScene(const std::filesystem::path& path)
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
        m_InspectorPanel.SetContext(sharedPtr);
    }

    void EditorLayer::SaveScene()
    {
        if(m_ScenePath.empty())
        {
            auto filepath = BeeEngine::FileDialogs::SaveFile({"BeeEngine Scene", "*.beescene"});
            if(filepath.empty())
            {
                BeeCoreError("Unable to save to file");
                return;
            }
            if(!filepath.ends_with(".beescene"))
                filepath += ".beescene";
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
}
