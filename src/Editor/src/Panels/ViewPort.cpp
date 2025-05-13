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
#include "Platform/RmlUi/RmlUi.hpp"
#include "Platform/RmlUi/RmlUi_Platform_SDL.h"
#include "Renderer/SceneRenderer.h"
#include "RmlUi/Core/Context.h"
#include "RmlUi/Core/DataModelHandle.h"
#include "RmlUi/Core/ElementDocument.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/SceneSerializer.h"
#include "Scripting/ScriptingEngine.h"
#include "Windowing/WindowHandler/WindowHandler.h"
#include "imgui.h"
#include <array>
#include <glm/gtc/type_ptr.hpp>

namespace BeeEngine::Editor
{
    template <typename T, typename R = int32_t>
    static R PhysicalSize(T size)
    {
        return static_cast<int32_t>(size) *
               static_cast<int32_t>(WindowHandler::GetInstance()->GetScaleFactor() +
                                    1.6); //+1.6 as dirty fix, because everything looks like shit in imgui :/
    }
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
                m_GameContext = RmlUi::CreateContext(
                    newProject->Name(),
                    {PhysicalSize(m_Width), PhysicalSize(m_Height)},
                    &newProject->GetProjectLocaleDomain()); // FIXME: Old context doesn't get deleted
            });
        FrameBufferPreferences preferences;
        preferences.Width = PhysicalSize(m_Width);
        preferences.Height = PhysicalSize(m_Height);
        preferences.Attachments = {
            FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RedInteger, FrameBufferTextureFormat::Depth24};

        preferences.Attachments.Attachments[1].TextureUsage = FrameBufferTextureUsage::CPUAndGPU; // RedInteger

        m_FrameBuffer = FrameBuffer::Create(preferences);
    }

    glm::mat4 MakeUIMatrix(float width, float height)
    {
        return glm::ortho(0.0f, width, height, 0.0f, -10000.f, 10000.f);
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
        if (m_GameContext && CurrentScene()->IsRuntime() && event.GetType() != EventType::MouseMoved &&
            IsMouseInViewport())
        {
            RmlUi::HandleEvents(m_GameContext, event, PhysicalSize<float, float>);
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
            mouseX = narrow_cast<int>(PhysicalSize(mouseX));
            mouseY = narrow_cast<int>(PhysicalSize(mouseY));
            ScriptingEngine::SetMousePosition(mouseX, mouseY);
        }
        RmlUi::SetMainContext(m_GameContext);

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
        m_GameContext = RmlUi::GetMainContext();
        if (m_GameContext)
        {
            cmd.Flush();
            RmlUi::UpdateAndRender(m_GameContext, cmd);
        }
        m_FrameBuffer->Unbind(cmd);
        RmlUi::SetMainContext(nullptr);
        if (IsMouseInViewport())
        {
            if (m_GameContext)
            {
                MouseMovedEvent mouseMovedEvent(m_MousePosition.x, m_MousePosition.y);
                EventDispatcher dispatcher{&mouseMovedEvent};
                RmlUi::HandleEvents(m_GameContext, dispatcher, PhysicalSize<float, float>);
            }
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

        m_FrameBuffer->Unbind(cmd);

        if (IsMouseInViewport())
        {
            int mouseX = narrow_cast<int>(PhysicalSize(mx));
            int mouseY = narrow_cast<int>(PhysicalSize(my));
            ScriptingEngine::SetMousePosition(mouseX, mouseY);
            m_HoveredEntity = GetHoveredEntity();
        }
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

        auto& io = ImGui::GetIO();
        auto logicalSize = ImGui::GetContentRegionAvail();

        if (narrow_cast<float>(m_Width) != logicalSize.x || narrow_cast<float>(m_Height) != logicalSize.y)
        {
            m_Width = narrow_cast<uint32_t>(logicalSize.x);
            m_Height = narrow_cast<uint32_t>(logicalSize.y);
            auto scaledWidth = PhysicalSize(logicalSize.x);
            auto scaledHeight = PhysicalSize(logicalSize.y);
            m_FrameBuffer->Resize(scaledWidth, scaledHeight);
            if (m_GameContext)
            {
                auto uimat4 = MakeUIMatrix(m_FrameBuffer->GetWidth(), m_FrameBuffer->GetHeight());
                Rml::Vector2i dimensions(scaledWidth, scaledHeight);
                m_GameContext->SetDimensions(dimensions);
                m_GameContext->SetDensityIndependentPixelRatio(PhysicalSize(logicalSize.x) /
                                                               static_cast<int32_t>(logicalSize.x));
                RmlUi::ResizeViewport(m_GameContext, {m_FrameBuffer->GetWidth(), m_FrameBuffer->GetHeight()});
            }
            CurrentScene()->OnViewPortResize(PhysicalSize(logicalSize.x), PhysicalSize(logicalSize.y));
            camera.SetViewportSize(PhysicalSize(logicalSize.x), PhysicalSize(logicalSize.y));
            ScriptingEngine::SetViewportSize(m_Width, m_Height);
        }
        auto textureID = m_FrameBuffer->GetColorAttachmentImGuiRendererID(0);
        BeeExpects(textureID != 0);
        ImGui::Image((ImTextureID)textureID, {static_cast<float>(m_Width), static_cast<float>(m_Height)});

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
        int mouseX = narrow_cast<int>(PhysicalSize(m_MousePosition.x));
        int mouseY = narrow_cast<int>(PhysicalSize(m_MousePosition.y));
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
