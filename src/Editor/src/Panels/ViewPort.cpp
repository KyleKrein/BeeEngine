//
// Created by alexl on 26.05.2023.
//

#include "ViewPort.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "gtc/type_ptr.hpp"
#include "Core/Events/Event.h"


namespace BeeEngine::Editor
{

    ViewPort::ViewPort(uint32_t width, uint32_t height, Entity& selectedEntity) noexcept
    : m_Width(width),
    m_Height(height),
    m_FrameBuffer(nullptr),
    m_IsFocused(false),
    m_IsHovered(false),
      m_Scene(std::move(CreateRef<Scene>())),
        m_SelectedEntity(selectedEntity)
    {
        FrameBufferPreferences preferences;
        preferences.Width = m_Width;
        preferences.Height = m_Height;
        preferences.Attachments = {FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::Depth24Stencil8};

        m_FrameBuffer = FrameBuffer::Create(preferences);
    }

    void ViewPort::OnEvent(EventDispatcher &event) noexcept
    {
        if(event.GetCategory() & EventCategory::App)
        {
            return;
        }
        DISPATCH_EVENT(event,MouseButtonPressedEvent ,EventType::MouseButtonPressed, OnMouseButtonPressed);
        //m_CameraController.OnEvent(event);
    }

    void ViewPort::UpdateRuntime() noexcept
    {
        m_FrameBuffer->Bind();
        //m_CameraController.OnUpdate();
        Renderer::Clear();
        m_Scene->UpdateRuntime();
        m_FrameBuffer->Unbind();
    }
    void ViewPort::UpdateEditor(EditorCamera& camera) noexcept
    {
        m_FrameBuffer->Bind();
        //m_CameraController.OnUpdate();
        Renderer::Clear();
        m_Scene->UpdateEditor(camera);
        m_FrameBuffer->Unbind();
    }

    void ViewPort::RenderImGuizmo(EditorCamera& camera)
    {
        //BeeCoreTrace("RenderImGuizmo. Mode: {}", m_GuizmoOperation == GuizmoOperation::None ? "None" : m_GuizmoOperation == GuizmoOperation::Translate ? "Translate" : m_GuizmoOperation == GuizmoOperation::Rotate ? "Rotate" : "Scale");
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        const auto windowPos = ImGui::GetWindowPos();
        const float windowWidth = ImGui::GetWindowWidth();
        const float windowHeight = ImGui::GetWindowHeight();
        ImGuizmo::SetRect(windowPos.x, windowPos.y,
                          gsl::narrow_cast<float>(windowWidth),
                          gsl::narrow_cast<float>(windowHeight));



        const glm::mat4& cameraProjection = camera.GetProjectionMatrix();
        glm::mat4 cameraView = camera.GetViewMatrix();

        auto& transformComponent = m_SelectedEntity.GetComponent<TransformComponent>();

        glm::mat4 transform = transformComponent.GetTransform();

        //Snapping
        const float snapValue = m_GuizmoOperation == GuizmoOperation::Rotate ? 45.0f : 0.5f;

        float snapValues[3] = {snapValue, snapValue, snapValue};

        ImGuizmo::Manipulate(glm::value_ptr(cameraView),
                             glm::value_ptr(cameraProjection),
                             static_cast<ImGuizmo::OPERATION>(m_GuizmoOperation),
                             ImGuizmo::LOCAL,
                             glm::value_ptr(transform),
                             nullptr,
                             m_GuizmoSnap ? snapValues : nullptr);
        if (ImGuizmo::IsUsing())
        {
            transformComponent.SetTransform(transform);
        }
    }

    void ViewPort::Render(EditorCamera& camera) noexcept
    {
        m_FrameBuffer->Bind();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("Viewport");
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
            camera.SetViewportSize(m_Width, m_Height);
        }
        ImGui::Image((ImTextureID)m_FrameBuffer->GetColorAttachmentRendererID(0), {size.x, size.y}, ImVec2{0, 1}, ImVec2{1, 0});
        OnMouseButtonPressed(nullptr);
        if (m_SelectedEntity != Entity::Null && m_GuizmoOperation != GuizmoOperation::None)
        {
            RenderImGuizmo(camera);
        }
        m_IsFocused = ImGui::IsWindowFocused();
        m_IsHovered = ImGui::IsWindowHovered();




        if(!m_IsFocused && !m_IsHovered)
        {
            //m_CameraController.Disable();
        }
        else
        {
            //m_CameraController.Enable();
        }
        ImGui::End();
        ImGui::PopStyleVar();
        m_FrameBuffer->Unbind();
    }

    bool ViewPort::OnMouseButtonPressed(MouseButtonPressedEvent* event) noexcept
    {
        bool shift = Input::KeyPressed(Key::LeftShift) || Input::KeyPressed(Key::RightShift);
        bool control = Input::KeyPressed(Key::LeftControl) || Input::KeyPressed(Key::RightControl);
        if(Input::KeyPressed(Key::T) && shift)
        {
            m_GuizmoOperation = GuizmoOperation::Translate;
        }
        else if(Input::KeyPressed(Key::R) && shift)
        {
            m_GuizmoOperation = GuizmoOperation::Rotate;
        }
        else if(Input::KeyPressed(Key::E) && shift)
        {
            m_GuizmoOperation = GuizmoOperation::Scale;
        }
        else if(Input::KeyPressed(Key::Q) && shift)
        {
            m_GuizmoOperation = GuizmoOperation::None;
        }
        if (control)
        {
            m_GuizmoSnap = true;
        } else
        {
            m_GuizmoSnap = false;
        }
        return false;
    }
}
