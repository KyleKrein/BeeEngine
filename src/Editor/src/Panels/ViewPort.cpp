//
// Created by alexl on 26.05.2023.
//

#include "ViewPort.h"
#include "Debug/Instrumentor.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "gtc/type_ptr.hpp"
#include "Core/Events/Event.h"
#include "Scene/SceneSerializer.h"
#include "Core/ResourceManager.h"
#include "Renderer/SceneRenderer.h"


namespace BeeEngine::Editor
{

    ViewPort::ViewPort(uint32_t width, uint32_t height, Entity& selectedEntity, const Color4& clearColor) noexcept
    : m_Width(width),
    m_Height(height),
    m_FrameBuffer(nullptr),
    m_IsFocused(false),
    m_IsHovered(false),
      m_Scene(std::move(CreateRef<Scene>())),
        m_SelectedEntity(selectedEntity),
        m_ClearColor(clearColor)
    {
        FrameBufferPreferences preferences;
        preferences.Width = m_Width;
        preferences.Height = m_Height;
        preferences.Attachments = {FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RedInteger, FrameBufferTextureFormat::Depth24};

        m_FrameBuffer = FrameBuffer::Create(preferences);
    }

    void ViewPort::OnEvent(EventDispatcher &event) noexcept
    {
        if(!m_IsFocused && !m_IsHovered)
            return;

        if(event.GetCategory() & EventCategory::App)
        {
            return;
        }
        DISPATCH_EVENT(event,MouseButtonPressedEvent ,EventType::MouseButtonPressed, OnMouseButtonPressed);
        DISPATCH_EVENT(event,KeyPressedEvent,EventType::KeyPressed, OnKeyButtonPressed);
        //m_CameraController.OnEvent(event);
    }

    void ViewPort::UpdateRuntime() noexcept
    {
        BEE_PROFILE_FUNCTION();
        Renderer::Clear();
        m_FrameBuffer->Bind();

        m_Scene->UpdateRuntime();
        SceneRenderer::RenderScene(*m_Scene, *m_FrameBuffer, m_GameDomain->GetLocale());

        m_FrameBuffer->Unbind();
    }
    void ViewPort::UpdateEditor(EditorCamera& camera) noexcept
    {
        BEE_PROFILE_FUNCTION();
        m_FrameBuffer->Bind();

        SceneRenderer::RenderScene(*m_Scene, *m_FrameBuffer, m_GameDomain->GetLocale(), camera.GetViewProjection());

        auto [mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        const glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];

        if(Renderer::GetAPI() == RenderAPI::OpenGL)
        {
            my = viewportSize.y - my;
        }

        int mouseX = static_cast<int>(mx);
        int mouseY = static_cast<int>(my);

        if(mouseX >= 0 && mouseY >= 0
        && gsl::narrow_cast<float>(mouseX) < viewportSize.x
        && gsl::narrow_cast<float>(mouseY) < viewportSize.y)
        {
            int pixelData = m_FrameBuffer->ReadPixel(1, mouseX, mouseY);
            pixelData--; //I make it -1 because entt starts from 0 and clear value for red integer in webgpu is 0 and I need to make invalid number -1 too, so in scene I make + 1
            //BeeCoreTrace("Coords: {}, {}. Pixel data: {}", mouseX, mouseY, pixelData);
            m_HoveredEntity = pixelData == -1 ? Entity::Null : Entity(EntityID{(entt::entity)pixelData}, m_Scene.get());
        }

        m_FrameBuffer->Unbind();
    }

    void ViewPort::RenderImGuizmo(EditorCamera& camera)
    {
        //BeeCoreTrace("RenderImGuizmo. Mode: {}", m_GuizmoOperation == GuizmoOperation::None ? "None" : m_GuizmoOperation == GuizmoOperation::Translate ? "Translate" : m_GuizmoOperation == GuizmoOperation::Rotate ? "Rotate" : "Scale");

        const glm::mat4* cameraProjection = nullptr;
        glm::mat4 cameraView;
        if(m_Scene->IsRuntime())
        {
            Entity mainCamera = m_Scene->GetPrimaryCameraEntity();
            if(!mainCamera)
                return;
            auto& cameraComponent = mainCamera.GetComponent<CameraComponent>();
            cameraProjection = &cameraComponent.Camera.GetProjectionMatrix();
            cameraView = glm::inverse(mainCamera.GetComponent<TransformComponent>().GetTransform());
        }
        else
        {
            cameraProjection = &camera.GetProjectionMatrix();
            cameraView = camera.GetViewMatrix();
        }
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);


        auto& transformComponent = m_SelectedEntity.GetComponent<TransformComponent>();

        glm::mat4 transform = Math::ToGlobalTransform(m_SelectedEntity);

        //Snapping
        const float snapValue = m_GuizmoOperation == GuizmoOperation::Rotate ? 45.0f : 0.5f;

        float snapValues[3] = {snapValue, snapValue, snapValue};

        ImGuizmo::Manipulate(glm::value_ptr(cameraView),
                             glm::value_ptr(*cameraProjection),
                             static_cast<ImGuizmo::OPERATION>(m_GuizmoOperation),
                             ImGuizmo::LOCAL,
                             glm::value_ptr(transform),
                             nullptr,
                             m_GuizmoSnap ? snapValues : nullptr);
        if (ImGuizmo::IsUsing())
        {
            transformComponent.SetTransform(Math::ToLocalTransform(m_SelectedEntity, transform));
        }
    }

    void ViewPort::Render(EditorCamera& camera) noexcept
    {
        //m_FrameBuffer->Bind();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("##Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos();
        m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        m_IsFocused = ImGui::IsWindowFocused();
        m_IsHovered = ImGui::IsWindowHovered();

        auto size = ImGui::GetContentRegionAvail();
        size.x = size.x > 0 ? size.x : 1;
        size.y = size.y > 0 ? size.y : 1;
        if(gsl::narrow_cast<float>(m_Width) != size.x || gsl::narrow_cast<float>(m_Height) != size.y)
        {
            m_Width = gsl::narrow_cast<uint32_t>(size.x);
            m_Height = gsl::narrow_cast<uint32_t>(size.y);
            m_FrameBuffer->Resize(m_Width, m_Height);
            m_Scene->OnViewPortResize(m_Width, m_Height);
            camera.SetViewportSize(m_Width, m_Height);
        }
        auto textureID = m_FrameBuffer->GetColorAttachmentRendererID(0);
        BeeExpects(textureID != 0);
        ImGui::Image((ImTextureID)textureID, {static_cast<float>(m_Width), static_cast<float>(m_Height)}/*, ImVec2{0, 1}, ImVec2{1, 0}*/);

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                Path p = m_WorkingDirectory / (const char*)payload->Data;
                if(ResourceManager::IsSceneExtension(p.GetExtension()))
                {
                    OpenScene(p);
                }
            }
            ImGui::EndDragDropTarget();
        }

        if (m_SelectedEntity != Entity::Null && m_GuizmoOperation != GuizmoOperation::None)
        {
            RenderImGuizmo(camera);
        }

        ImGui::End();
        ImGui::PopStyleVar();
        //m_FrameBuffer->Unbind();
    }

    bool ViewPort::OnKeyButtonPressed(KeyPressedEvent* event) noexcept
    {
        bool shift = Input::KeyPressed(Key::LeftShift) || Input::KeyPressed(Key::RightShift);
        bool control = Input::KeyPressed(Key::LeftControl) || Input::KeyPressed(Key::RightControl);
        if(Input::KeyPressed(Key::T) && shift)
        {
            if (!ImGuizmo::IsUsing())
                m_GuizmoOperation = GuizmoOperation::Translate;
        }
        else if(Input::KeyPressed(Key::R) && shift)
        {
            if (!ImGuizmo::IsUsing())
                m_GuizmoOperation = GuizmoOperation::Rotate;
        }
        else if(Input::KeyPressed(Key::E) && shift)
        {
            if (!ImGuizmo::IsUsing())
                m_GuizmoOperation = GuizmoOperation::Scale;
        }
        else if(Input::KeyPressed(Key::Q) && shift)
        {
            if (!ImGuizmo::IsUsing())
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

    bool ViewPort::OnMouseButtonPressed(MouseButtonPressedEvent *event) noexcept
    {
        if(event->GetButton() == MouseButton::Left)
        {
            if(!m_Scene->IsRuntime() && m_IsHovered && !ImGuizmo::IsOver() && !Input::KeyPressed(Key::LeftAlt))
            {
                m_SelectedEntity = m_HoveredEntity;
            }
        }
        return false;
    }

    void ViewPort::OpenScene(const Path& path)
    {
        BeeCoreTrace("Opening scene {0}", path.AsUTF8());
        m_NewSceneWasLoaded = true;
        m_ScenePath = path.AsUTF8();
    }
}
