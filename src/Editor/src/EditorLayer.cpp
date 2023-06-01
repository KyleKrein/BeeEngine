//
// Created by alexl on 26.05.2023.
//

#include "EditorLayer.h"


namespace BeeEngine::Editor
{

    void EditorLayer::OnAttach() noexcept
    {
        Renderer::SetClearColor(Color4::Black);
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
        m_FpsCounter.Render();
        m_DockSpace.End();
    }

    void EditorLayer::OnEvent(EventDispatcher &event) noexcept
    {
        m_ViewPort.OnEvent(event);
    }
    constexpr RectangleProperties rectangle(0, 0, 0, 1, 1, 0, Color4::Green);
    void EditorLayer::DrawToScene()
    {
        Renderer2D::DrawRectangle(rectangle);
    }
}
