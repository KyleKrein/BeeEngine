//
// Created by alexl on 26.05.2023.
//

#include "ViewPort.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"


namespace BeeEngine::Editor
{

    ViewPort::ViewPort(uint32_t width, uint32_t height) noexcept
    : m_Width(width),
    m_Height(height),
    m_FrameBuffer(FrameBuffer::Create({width, height})),
    m_IsFocused(false),
    m_IsHovered(false),
      m_Scene(std::move(CreateRef<Scene>()))
    {
        auto camera = m_Scene->CreateEntity("Scene Camera");
        auto& cameraComponent = camera.AddComponent<CameraComponent>();
        cameraComponent.Primary = true;
        cameraComponent.Camera.SetViewportSize(width, height);
    }

    void ViewPort::OnEvent(EventDispatcher &event) noexcept
    {
        if(event.GetCategory() & EventCategory::App)
        {
            return;
        }
        //m_CameraController.OnEvent(event);
    }

    void ViewPort::Update() noexcept
    {
        m_FrameBuffer->Bind();
        //m_CameraController.OnUpdate();
        Renderer::Clear();
        m_Scene->OnUpdate();
        /*Renderer2D::BeginScene(m_CameraController);
        m_RenderFunction();
        Renderer2D::EndScene();*/
        m_FrameBuffer->Unbind();
    }

    void ViewPort::Render() noexcept
    {
        m_FrameBuffer->Bind();
        ImGui::Begin("Viewport");
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        auto size = ImGui::GetContentRegionAvail();
        size.x = size.x > 0 ? size.x : 1;
        size.y = size.y > 0 ? size.y : 1;
        if(gsl::narrow_cast<float>(m_Width) != size.x || gsl::narrow_cast<float>(m_Height) != size.y)
        {
            m_Width = gsl::narrow_cast<uint32_t>(size.x);
            m_Height = gsl::narrow_cast<uint32_t>(size.y);
            m_FrameBuffer->Resize(m_Width, m_Height);
            auto event = CreateScope<WindowResizeEvent>(m_Width, m_Height);
            EventDispatcher dispatcher(event.get());
            //m_CameraController.OnEvent(dispatcher);
            m_Scene->OnViewPortResize(m_Width, m_Height);
        }
        ImGui::Image((ImTextureID)m_FrameBuffer->GetColorAttachmentRendererID(), {size.x, size.y}, ImVec2{0, 1}, ImVec2{1, 0});
        ImGui::PopStyleVar();
        m_IsFocused = ImGui::IsWindowFocused();
        m_IsHovered = ImGui::IsWindowHovered();
        if(!m_IsFocused || !m_IsHovered)
        {
            //m_CameraController.Disable();
        }
        else
        {
            //m_CameraController.Enable();
        }
        ImGui::End();
        m_FrameBuffer->Unbind();
    }
}
