//
// Created by alexl on 26.05.2023.
//

#include "EditorLayer.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Utils/FileDialogs.h"


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
        }
        m_FpsCounter.Update();
    }

    void EditorLayer::OnGUIRendering() noexcept
    {
        m_DockSpace.Start();
        m_MenuBar.Render();
        m_ViewPort.Render(m_EditorCamera);
        m_SceneHierarchyPanel.OnGUIRender();
        m_InspectorPanel.OnGUIRender(m_SceneHierarchyPanel.GetSelectedEntity());
        m_AssetPanel.OnGUIRender();
        m_FpsCounter.Render();
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
