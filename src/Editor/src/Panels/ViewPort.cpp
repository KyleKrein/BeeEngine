//
// Created by alexl on 26.05.2023.
//

#include "ViewPort.h"
#include "AssetPanel.h"
#include "Core/AssetManagement/Asset.h"
#include "Core/AssetManagement/AssetManager.h"
#include "Core/AssetManagement/EditorAssetManager.h"
#include "Core/CodeSafety/Expects.h"
#include "Core/Events/Event.h"
#include "Core/Logging/Log.h"
#include "Core/ResourceManager.h"
#include "Debug/Instrumentor.h"
#include "Gui/ImGui/ImGuiExtension.h"
#include "Renderer/SceneRenderer.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/SceneSerializer.h"
#include "Scripting/ScriptingEngine.h"
#include "Windowing/WindowHandler/WindowHandler.h"
#include "gtc/type_ptr.hpp"
#include "imgui.h"

namespace BeeEngine::Editor
{

    ViewPort::ViewPort(Property<Scope<ProjectFile>>& project,
                       uint32_t width,
                       uint32_t height,
                       Entity& selectedEntity,
                       const Color4& clearColor,
                       EditorAssetManager& assetManager) noexcept
        : m_Width(width),
          m_Height(height),
          m_FrameBuffer(nullptr),
          m_IsFocused(false),
          m_IsHovered(false),
          CurrentScene(std::move(CreateRef<Scene>())),
          m_SelectedEntity(selectedEntity),
          m_ClearColor(clearColor),
          m_AssetManager(assetManager)
    {
        project.valueChanged().connect(
            [this](const auto& newProject)
            {
                m_WorkingDirectory = newProject->FolderPath.get();
                m_GameDomain = &newProject->GetProjectLocaleDomain();
            });
        FrameBufferPreferences preferences;
        preferences.Width = m_Width * WindowHandler::GetInstance()->GetScaleFactor();
        preferences.Height = m_Height * WindowHandler::GetInstance()->GetScaleFactor();
        preferences.Attachments = {
            FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RedInteger, FrameBufferTextureFormat::Depth24};

        preferences.Attachments.Attachments[1].TextureUsage = FrameBufferTextureUsage::CPUAndGPU; // RedInteger

        m_FrameBuffer = FrameBuffer::Create(preferences);
    }

    void ViewPort::OnEvent(EventDispatcher& event) noexcept
    {
        if (!m_IsFocused && !m_IsHovered)
            return;
        if (!CurrentScene()->IsRuntime() && m_LastHoveredRuntime)
        {
            m_LastHoveredRuntime = Entity::Null;
        }
        if (event.GetCategory() & EventCategory::App)
        {
            return;
        }
        event.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& event) -> bool
                                                { return OnMouseButtonPressed(&event); });
        event.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& event) -> bool { return OnKeyButtonPressed(&event); });
        // m_CameraController.OnEvent(event);
    }

    void ViewPort::UpdateRuntime(bool renderPhysicsColliders) noexcept
    {
        BEE_PROFILE_FUNCTION();
        auto cmd = m_FrameBuffer->Bind();

        auto [mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        const glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];

        int mouseX = static_cast<int>(mx);
        int mouseY = static_cast<int>(my);

        m_MousePosition = {mx, my};

        if (IsMouseInViewport())
        {
            mouseX = gsl::narrow_cast<int>(mouseX * WindowHandler::GetInstance()->GetScaleFactor());
            mouseY = gsl::narrow_cast<int>(mouseY * WindowHandler::GetInstance()->GetScaleFactor());
            ScriptingEngine::SetMousePosition(mouseX, mouseY);
        }

        CurrentScene()->UpdateRuntime();
        if (m_SelectedEntity && !m_SelectedEntity.IsValid())
        {
            m_SelectedEntity = Entity::Null;
        }
        SceneRenderer::RenderScene(*CurrentScene(), cmd, m_GameDomain->GetLocale());

        auto primaryCameraEntity = CurrentScene()->GetPrimaryCameraEntity();
        if (primaryCameraEntity)
        {
            auto& cameraComponent = primaryCameraEntity.GetComponent<CameraComponent>();
            auto& camera = cameraComponent.Camera;
            auto viewMatrix = glm::inverse(Math::ToGlobalTransform(primaryCameraEntity));
            auto viewProjection = camera.GetProjectionMatrix() * viewMatrix;
            m_CameraUniformBuffer->SetData((glm::value_ptr(viewProjection)), sizeof(glm::mat4));
            RenderSelectedEntityOutline(cmd);
            if (renderPhysicsColliders)
                SceneRenderer::RenderPhysicsColliders(*CurrentScene(), cmd, *m_CameraBindingSet);
        }
        m_FrameBuffer->Unbind(cmd);
        if (IsMouseInViewport())
        {
            Entity hovered = GetHoveredEntity();
            if (hovered != m_LastHoveredRuntime)
            {
                if (m_LastHoveredRuntime && m_LastHoveredRuntime.IsValid())
                {
                    ScriptingEngine::OnMouseLeave(m_LastHoveredRuntime.GetUUID());
                }
                m_LastHoveredRuntime = hovered;
                if (m_LastHoveredRuntime)
                {
                    ScriptingEngine::OnMouseEnter(m_LastHoveredRuntime.GetUUID());
                }
            }
        }
    }
    void ViewPort::UpdateEditor(EditorCamera& camera, bool renderPhysicsColliders) noexcept
    {
        BEE_PROFILE_FUNCTION();
        auto cmd = m_FrameBuffer->Bind();
        auto viewProjection = camera.GetViewProjection();
        m_CameraUniformBuffer->SetData(glm::value_ptr(viewProjection), sizeof(glm::mat4));
        if (m_SelectedEntity && m_SelectedEntity.HasComponent<CameraComponent>())
            RenderCameraFrustum(cmd);
        SceneRenderer::RenderScene(*CurrentScene(),
                                   cmd,
                                   m_GameDomain->GetLocale(),
                                   camera,
                                   camera.GetViewProjection(),
                                   camera.GetPosition(),
                                   camera.GetForwardDirection(),
                                   camera.GetUpDirection(),
                                   camera.GetRightDirection());
        RenderSelectedEntityOutline(cmd);
        if (renderPhysicsColliders)
            SceneRenderer::RenderPhysicsColliders(*CurrentScene(), cmd, *m_CameraBindingSet);
        auto [mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        m_MousePosition = {mx, my};

        if (IsMouseInViewport())
        {
            int mouseX = gsl::narrow_cast<int>(mx * WindowHandler::GetInstance()->GetScaleFactor());
            int mouseY = gsl::narrow_cast<int>(my * WindowHandler::GetInstance()->GetScaleFactor());
            ScriptingEngine::SetMousePosition(mouseX, mouseY);
            m_HoveredEntity = GetHoveredEntity();
        }
        m_FrameBuffer->Unbind(cmd);
    }

    void ViewPort::RenderImGuizmo(EditorCamera& camera)
    {
        // BeeCoreTrace("RenderImGuizmo. Mode: {}", m_GuizmoOperation == GuizmoOperation::None ? "None" :
        // m_GuizmoOperation == GuizmoOperation::Translate ? "Translate" : m_GuizmoOperation == GuizmoOperation::Rotate
        // ? "Rotate" : "Scale");

        const glm::mat4* cameraProjection = nullptr;
        glm::mat4 cameraView;
        if (CurrentScene()->IsRuntime())
        {
            Entity mainCamera = CurrentScene()->GetPrimaryCameraEntity();
            if (!mainCamera)
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

        ImGuizmo::SetRect(m_ViewportBounds[0].x,
                          m_ViewportBounds[0].y,
                          m_ViewportBounds[1].x - m_ViewportBounds[0].x,
                          m_ViewportBounds[1].y - m_ViewportBounds[0].y);

        auto& transformComponent = m_SelectedEntity.GetComponent<TransformComponent>();

        glm::mat4 transform = Math::ToGlobalTransform(m_SelectedEntity);

        // Snapping
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
        // m_FrameBuffer->Bind();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("##Viewport",
                     nullptr,
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoTitleBar);

        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos();
        m_ViewportBounds[0] = {viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y};
        m_ViewportBounds[1] = {viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y};

        m_IsFocused = ImGui::IsWindowFocused();
        m_IsHovered = ImGui::IsWindowHovered();

        auto size = ImGui::GetContentRegionAvail();
        size.x = size.x > 0 ? size.x : 1;
        size.y = size.y > 0 ? size.y : 1;
        if (gsl::narrow_cast<float>(m_Width) != size.x || gsl::narrow_cast<float>(m_Height) != size.y)
        {
            m_Width = gsl::narrow_cast<uint32_t>(size.x);
            m_Height = gsl::narrow_cast<uint32_t>(size.y);
            m_FrameBuffer->Resize(m_Width * WindowHandler::GetInstance()->GetScaleFactor(),
                                  m_Height * WindowHandler::GetInstance()->GetScaleFactor());
            CurrentScene()->OnViewPortResize(m_Width, m_Height);
            camera.SetViewportSize(m_Width, m_Height);
            ScriptingEngine::SetViewportSize(m_Width, m_Height);
        }
        auto textureID = m_FrameBuffer->GetColorAttachmentImGuiRendererID(0);
        BeeExpects(textureID != 0);
        ImGui::Image((ImTextureID)textureID,
                     {static_cast<float>(m_Width), static_cast<float>(m_Height)} /*, ImVec2{0, 1}, ImVec2{1, 0}*/);

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                Path p = m_WorkingDirectory / (const char*)payload->Data;
                if (ResourceManager::IsSceneExtension(p.GetExtension()))
                {
                    OpenScene(p);
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::AcceptDragAndDrop<AssetHandle>(AssetPanel::GetDragAndDropTypeName(AssetType::Scene),
                                              [this](const AssetHandle& handle)
                                              {
                                                  BeeExpects(m_AssetManager.IsAssetHandleValid(handle));
                                                  const auto& sceneMetadata = m_AssetManager.GetAssetMetadata(handle);
                                                  BeeExpects(sceneMetadata.Location == AssetLocation::FileSystem);
                                                  OpenScene(std::get<Path>(sceneMetadata.Data));
                                              });

        if (m_SelectedEntity != Entity::Null && m_GuizmoOperation != GuizmoOperation::None)
        {
            RenderImGuizmo(camera);
        }

        ImGui::End();
        ImGui::PopStyleVar();
        // m_FrameBuffer->Unbind();
    }

    bool ViewPort::OnKeyButtonPressed(KeyPressedEvent* event) noexcept
    {
        bool shift = Input::KeyPressed(Key::LeftShift) || Input::KeyPressed(Key::RightShift);
        bool control = Input::KeyPressed(Key::LeftControl) || Input::KeyPressed(Key::RightControl);
        if (Input::KeyPressed(Key::T) && shift)
        {
            if (!ImGuizmo::IsUsing())
                m_GuizmoOperation = GuizmoOperation::Translate;
        }
        else if (Input::KeyPressed(Key::R) && shift)
        {
            if (!ImGuizmo::IsUsing())
                m_GuizmoOperation = GuizmoOperation::Rotate;
        }
        else if (Input::KeyPressed(Key::E) && shift)
        {
            if (!ImGuizmo::IsUsing())
                m_GuizmoOperation = GuizmoOperation::Scale;
        }
        else if (Input::KeyPressed(Key::Q) && shift)
        {
            if (!ImGuizmo::IsUsing())
                m_GuizmoOperation = GuizmoOperation::None;
        }
        if (control)
        {
            m_GuizmoSnap = true;
        }
        else
        {
            m_GuizmoSnap = false;
        }
        return false;
    }

    bool ViewPort::OnMouseButtonPressed(MouseButtonPressedEvent* event) noexcept
    {
        /*if(event->GetButton() == MouseButton::Left && !m_Scene->IsRuntime()
        && IsMouseInViewport() && !m_IsReadPixelTaskRunning && !ImGuizmo::IsOver()
        && !Input::KeyPressed(Key::LeftAlt))
        {
            m_ReadPixelTask = m_FrameBuffer->ReadPixel(1, m_MousePosition.x, m_MousePosition.y);
            m_IsReadPixelTaskRunning = true;
        }*/
        if (event->GetButton() == MouseButton::Left)
        {
            if (!CurrentScene()->IsRuntime() && m_IsHovered &&
                (!ImGuizmo::IsOver() || m_SelectedEntity == Entity::Null) && !Input::KeyPressed(Key::LeftAlt))
            {
                m_SelectedEntity = m_HoveredEntity;
            }
        }
        if (CurrentScene()->IsRuntime())
        {
            if (IsMouseInViewport())
            {
                Entity clicked = GetHoveredEntity();
                if (clicked)
                {
                    ScriptingEngine::OnMouseClick(clicked.GetUUID(), event->GetButton());
                }
            }
        }
        return false;
    }

    Entity ViewPort::GetHoveredEntity()
    {
        int mouseX = gsl::narrow_cast<int>(m_MousePosition.x * WindowHandler::GetInstance()->GetScaleFactor());
        int mouseY = gsl::narrow_cast<int>(m_MousePosition.y * WindowHandler::GetInstance()->GetScaleFactor());
        int pixelData = m_FrameBuffer->ReadPixel(1, mouseX, mouseY);
        pixelData--; // I make it -1 because entt starts from 0 and clear value for red integer in webgpu is
                     // 0 and I need to make invalid number -1 too, so in scene I make + 1
        if (pixelData == -1)
        {
            return Entity::Null;
        }
        return {EntityID{(entt::entity)pixelData}, CurrentScene()};
    }

    void ViewPort::OpenScene(const Path& path)
    {
        BeeCoreTrace("Opening scene {0}", path);
        m_NewSceneWasLoaded = true;
        const auto* handlePtr = m_AssetManager.GetAssetHandleByName(path.GetFileNameWithoutExtension());
        BeeExpects(handlePtr);
        m_SceneHandle = *handlePtr;
    }

    void ViewPort::RenderCameraFrustum(CommandBuffer& commandBuffer)
    {
        auto& transformComponent = m_SelectedEntity.GetComponent<TransformComponent>();
        auto& cameraComponent = m_SelectedEntity.GetComponent<CameraComponent>();
        auto& camera = cameraComponent.Camera;
        auto transform = Math::ToGlobalTransform(m_SelectedEntity);
        float aspectRatio = camera.GetAspectRatio();
        float fovY = camera.GetVerticalFOV();
        float zNear = camera.GetNearClip();
        float zFar = camera.GetFarClip();
        Color4 color = Color4::Green;

        // Вычисление ширины и высоты ближних и дальних плоскостей фрустума
        float nearHeight = tan(fovY) / 2.0f * zNear * 2.0f;
        float nearWidth = nearHeight * aspectRatio;
        float farHeight = tan(fovY / 2.0f) * zFar * 2.0f;
        float farWidth = farHeight * aspectRatio;

        // Вычисление вершин фрустума в локальных координатах камеры
        glm::vec3 ntl =
            glm::vec3(-nearWidth / 2.0f, nearHeight / 2.0f, zNear); // TODO: must be -zNear and -zFar everywhere
        glm::vec3 ntr = glm::vec3(nearWidth / 2.0f, nearHeight / 2.0f, zNear);
        glm::vec3 nbl = glm::vec3(-nearWidth / 2.0f, -nearHeight / 2.0f, zNear);
        glm::vec3 nbr = glm::vec3(nearWidth / 2.0f, -nearHeight / 2.0f, zNear);

        glm::vec3 ftl = glm::vec3(-farWidth / 2.0f, farHeight / 2.0f, zFar);
        glm::vec3 ftr = glm::vec3(farWidth / 2.0f, farHeight / 2.0f, zFar);
        glm::vec3 fbl = glm::vec3(-farWidth / 2.0f, -farHeight / 2.0f, zFar);
        glm::vec3 fbr = glm::vec3(farWidth / 2.0f, -farHeight / 2.0f, zFar);

        // Преобразование вершин в мировые координаты
        ntl = glm::vec3(transform * glm::vec4(ntl, 1.0f));
        ntr = glm::vec3(transform * glm::vec4(ntr, 1.0f));
        nbl = glm::vec3(transform * glm::vec4(nbl, 1.0f));
        nbr = glm::vec3(transform * glm::vec4(nbr, 1.0f));

        ftl = glm::vec3(transform * glm::vec4(ftl, 1.0f));
        ftr = glm::vec3(transform * glm::vec4(ftr, 1.0f));
        fbl = glm::vec3(transform * glm::vec4(fbl, 1.0f));
        fbr = glm::vec3(transform * glm::vec4(fbr, 1.0f));

        // Отрисовка линий фрустума
        // Верхняя грань
        commandBuffer.SubmitLine(ntl, ntr, *m_CameraBindingSet, color, 0.1f);
        commandBuffer.SubmitLine(ntr, ftr, *m_CameraBindingSet, color, 0.1f);
        commandBuffer.SubmitLine(ftr, ftl, *m_CameraBindingSet, color, 0.1f);
        commandBuffer.SubmitLine(ftl, ntl, *m_CameraBindingSet, color, 0.1f);

        // Нижняя грань
        commandBuffer.SubmitLine(nbl, nbr, *m_CameraBindingSet, color, 0.1f);
        commandBuffer.SubmitLine(nbr, fbr, *m_CameraBindingSet, color, 0.1f);
        commandBuffer.SubmitLine(fbr, fbl, *m_CameraBindingSet, color, 0.1f);
        commandBuffer.SubmitLine(fbl, nbl, *m_CameraBindingSet, color, 0.1f);

        // Боковые грани
        commandBuffer.SubmitLine(ntl, nbl, *m_CameraBindingSet, color, 0.1f);
        commandBuffer.SubmitLine(ntr, nbr, *m_CameraBindingSet, color, 0.1f);
        commandBuffer.SubmitLine(ftl, fbl, *m_CameraBindingSet, color, 0.1f);
        commandBuffer.SubmitLine(ftr, fbr, *m_CameraBindingSet, color, 0.1f);
    }

    void ViewPort::RenderSelectedEntityOutline(CommandBuffer& commandBuffer)
    {
        if (m_SelectedEntity && (m_SelectedEntity.HasComponent<SpriteRendererComponent>() ||
                                 m_SelectedEntity.HasComponent<CircleRendererComponent>()))
        {
            auto transform = Math::ToGlobalTransform(m_SelectedEntity);
            commandBuffer.DrawRect(transform, Color4::DarkOrange, *m_CameraBindingSet, 0.05f);
        }
    }

    void ViewPort::HandleReadPixelTask()
    {
        /*if(!m_IsReadPixelTaskRunning)
        {
            return;
        }
        int pixelData = sync_await(std::move(m_ReadPixelTask));//m_ReadPixelTask.get();
        m_IsReadPixelTaskRunning = false;
        pixelData--; //I make it -1 because entt starts from 0 and clear value for red integer in webgpu is 0 and I need
        to make invalid number -1 too, so in scene I make + 1 auto hovered = pixelData == -1 ? Entity::Null :
        Entity(EntityID{(entt::entity)pixelData}, m_Scene.get()); if(hovered)
        {
            m_SelectedEntity = hovered;
        }*/
    }

    bool ViewPort::IsMouseInViewport()
    {
        const glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
        return m_MousePosition.x >= 0 && m_MousePosition.y >= 0 && m_MousePosition.x < viewportSize.x &&
               m_MousePosition.y < viewportSize.y;
    }
} // namespace BeeEngine::Editor
