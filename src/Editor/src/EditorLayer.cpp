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
        m_ViewPort.OnUpdate();
    }

    void EditorLayer::OnGUIRendering() noexcept
    {
        m_ViewPort.Render();
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
