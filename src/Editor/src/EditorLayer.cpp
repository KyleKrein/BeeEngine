//
// Created by alexl on 26.05.2023.
//

#include "EditorLayer.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"


namespace BeeEngine::Editor
{

    void EditorLayer::OnAttach() noexcept
    {
        Renderer::SetClearColor(Color4::Black);
        auto forestTexture = Texture2D::Create("Assets/Textures/forest.png");

        auto test = m_ViewPort.GetScene()->CreateEntity("Test");
        test.AddComponent<Texture2DComponent>(forestTexture);

        m_SceneHierarchyPanel.SetContext(m_ViewPort.GetScene());
        m_InspectorPanel.SetContext(m_ViewPort.GetScene());
    }

    void EditorLayer::OnDetach() noexcept
    {

    }

    void EditorLayer::OnUpdate() noexcept
    {
        Renderer::Clear();
        m_ViewPort.Update();
        m_FpsCounter.Update();
    }

    void EditorLayer::OnGUIRendering() noexcept
    {
        m_DockSpace.Start();
        m_ViewPort.Render();
        m_SceneHierarchyPanel.OnGUIRender();
        m_InspectorPanel.OnGUIRender(m_SceneHierarchyPanel.GetSelectedEntity());
        m_FpsCounter.Render();
        m_DockSpace.End();
    }

    void EditorLayer::OnEvent(EventDispatcher &event) noexcept
    {
        m_ViewPort.OnEvent(event);
    }
}
