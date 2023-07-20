//
// Created by alexl on 26.05.2023.
//

#include "EditorLayer.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Utils/FileDialogs.h"
#include "Core/ResourceManager.h"


namespace BeeEngine::Editor
{

    void EditorLayer::OnAttach() noexcept
    {
        SetUpMenuBar();
        //Renderer::SetClearColor(Color4::Black);
        //auto forestTexture = Texture2D::Create("Assets/Textures/forest.png");

        //auto test = m_ViewPort.GetScene()->CreateEntity("Test");
        //test.AddComponent<Texture2DComponent>(forestTexture);

        m_EditorCamera = EditorCamera(45.0f, 1.778f, 0.1f, 1000.0f);
        m_SceneHierarchyPanel.SetContext(m_ViewPort.GetScene());
        m_InspectorPanel.SetContext(m_ViewPort.GetScene());
    }

    void EditorLayer::OnDetach() noexcept
    {

    }
    void EditorLayer::OnUpdate() noexcept
    {
        m_EditorCamera.OnUpdate();
        if(m_IsRuntime)
        {
            m_ViewPort.UpdateRuntime();
        }
        else
        {
            m_ViewPort.UpdateEditor(m_EditorCamera);
            if(m_ViewPort.IsNewSceneLoaded())
            {
                m_SceneHierarchyPanel.SetContext(m_ViewPort.GetScene());
                m_InspectorPanel.SetContext(m_ViewPort.GetScene());
            }
        }
        m_FpsCounter.Update();
    }

    void EditorLayer::OnGUIRendering() noexcept
    {
        m_DockSpace.Start();
        if(m_ProjectFile)
        {
            m_MenuBar.Render();
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
                    return;
                }
                std::filesystem::path projectPath = (path);
                auto name = ResourceManager::GetNameFromFilePath(projectPath.string());
                path = projectPath.remove_filename().string();
                path.pop_back();
                m_ProjectFile = CreateScope<ProjectFile>(path, name);
                m_AssetPanel.SetWorkingDirectory(m_ProjectFile->GetProjectPath());
                m_ViewPort.SetWorkingDirectory(m_ProjectFile->GetProjectPath());
                m_InspectorPanel.SetWorkingDirectory(m_ProjectFile->GetProjectPath());
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
                std::filesystem::path projectPath = (path + ".beeproj");
                auto name = ResourceManager::GetNameFromFilePath(projectPath.string());
                path = projectPath.remove_filename().string();
                path.pop_back();
                m_ProjectFile = CreateScope<ProjectFile>(path, name);
                m_AssetPanel.SetWorkingDirectory(m_ProjectFile->GetProjectPath());
                m_ViewPort.SetWorkingDirectory(m_ProjectFile->GetProjectPath());
                m_InspectorPanel.SetWorkingDirectory(m_ProjectFile->GetProjectPath());
            }
            end:
            ImGui::End();
        }
        m_DockSpace.End();
    }

    void EditorLayer::OnEvent(EventDispatcher &event) noexcept
    {
        m_EditorCamera.OnEvent(event);
        m_ViewPort.OnEvent(event);
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
            m_SceneHierarchyPanel.ClearSelection();
            m_ViewPort.GetScene()->Clear();
            m_ScenePath = filepath;
            m_SceneSerializer.Deserialize(m_ScenePath);
            m_ViewPort.GetScene()->OnViewPortResize(m_ViewPort.GetWidth(), m_ViewPort.GetHeight());
        }});
        fileMenu.AddChild({"Save Scene", [this](){
            if(m_ScenePath.empty())
            {
                auto filepath = BeeEngine::FileDialogs::SaveFile({"BeeEngine Scene", "*.beescene"});
                if(filepath.empty())
                {
                    BeeCoreError("Unable to save to file");
                    return;
                }
                m_ScenePath = filepath;
                if(!m_ScenePath.ends_with(".beescene"))
                    m_ScenePath += ".beescene";
            }
            m_SceneSerializer.Serialize(m_ScenePath);
        }});
        fileMenu.AddChild({"Save Scene As...", [this](){
            auto filepath = BeeEngine::FileDialogs::SaveFile({"BeeEngine Scene", "*.beescene"});
            if(filepath.empty())
            {
                BeeCoreError("Unable to save to file");
                return;
            }
            m_ScenePath = filepath;
            if(!m_ScenePath.ends_with(".beescene"))
                m_ScenePath += ".beescene";
            m_SceneSerializer.Serialize(m_ScenePath);
        }});
        fileMenu.AddChild({"Exit", [](){BeeEngine::Application::GetInstance().Close();}});
        m_MenuBar.AddElement(fileMenu);
    }
}
