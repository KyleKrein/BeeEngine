//
// Created by alexl on 26.05.2023.
//

#include "ViewPort.h"


namespace BeeEngine::Editor
{

    ViewPort::ViewPort(uint32_t width, uint32_t height, const std::function<void()>& func) noexcept
    : m_Width(width),
    m_Height(height),
    m_FrameBuffer(FrameBuffer::Create({width, height})),
    m_IsFocused(false),
    m_IsHovered(false),
    m_CameraController(),
    m_RenderFunction(func)
    {}

    void ViewPort::OnEvent(EventDispatcher &event) noexcept
    {
        if(event.GetCategory() & EventCategory::App)
        {
            return;
        }
        m_CameraController.OnEvent(event);
    }

    void ViewPort::OnUpdate() noexcept
    {
        m_FrameBuffer->Bind();
        m_CameraController.OnUpdate();
        Renderer::Clear();
        Renderer2D::BeginScene(m_CameraController);
        m_RenderFunction();
        Renderer2D::EndScene();
        m_FrameBuffer->Unbind();
    }

    void ViewPort::Render() noexcept
    {
        m_FrameBuffer->Bind();
        ImGui::Begin("Viewport");
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        auto size = ImGui::GetContentRegionAvail();
        if(gsl::narrow_cast<float>(m_Width) != size.x || gsl::narrow_cast<float>(m_Height) != size.y)
        {
            m_Width = size.x;
            m_Height = size.y;
            m_FrameBuffer->Resize(m_Width, m_Height);
            auto event = CreateScope<WindowResizeEvent>(m_Width, m_Height);
            EventDispatcher dispatcher(event.get());
            m_CameraController.OnEvent(dispatcher);
        }
        ImGui::Image((ImTextureID)m_FrameBuffer->GetColorAttachmentRendererID(), {size.x, size.y}, ImVec2{0, 1}, ImVec2{1, 0});
        ImGui::PopStyleVar();
        m_IsFocused = ImGui::IsWindowFocused();
        m_IsHovered = ImGui::IsWindowHovered();
        if(!m_IsFocused || !m_IsHovered)
        {
            m_CameraController.Disable();
        }
        else
        {
            m_CameraController.Enable();
        }
        ImGui::End();
        m_FrameBuffer->Unbind();
    }
}
