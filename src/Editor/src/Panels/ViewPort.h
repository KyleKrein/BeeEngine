//
// Created by alexl on 26.05.2023.
//

#pragma once

#include "BeeEngine.h"
#include "Core/AssetManagement/Asset.h"
#include "Core/AssetManagement/EditorAssetManager.h"
#include "Gui/ImGui/IImGuiElement.h"
#include "Locale/Locale.h"
#include "ProjectFile.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Scene/SceneCamera.h"
#include "kdbindings/property.h"
#include <ImGuizmo.h>

namespace BeeEngine::Editor
{
    enum class GuizmoOperation
    {
        None = -1,
        Translate = ImGuizmo::OPERATION::TRANSLATE,
        Rotate = ImGuizmo::OPERATION::ROTATE,
        Scale = ImGuizmo::OPERATION::SCALE
    };
    class ViewPort final
    {
    public:
        ViewPort(Property<Scope<ProjectFile>>& project,
                 uint32_t width,
                 uint32_t height,
                 Entity& selectedEntity,
                 const Color4& clearColor,
                 EditorAssetManager& assetManager) noexcept;
        Property<Ref<Scene>> CurrentScene;
        void OnEvent(EventDispatcher& event) noexcept;
        void UpdateRuntime(bool renderPhysicsColliders) noexcept;
        void UpdateEditor(EditorCamera& camera, bool renderPhysicsColliders) noexcept;
        void Render(EditorCamera& camera) noexcept;
        bool ShouldHandleEvents() const noexcept { return m_IsFocused && m_IsHovered; }

        [[nodiscard]] uint32_t GetHeight() const { return m_Height; }
        [[nodiscard]] uint32_t GetWidth() const { return m_Width; }

        bool IsNewSceneLoaded() const noexcept
        {
            bool tmp = m_NewSceneWasLoaded;
            m_NewSceneWasLoaded = false;
            return tmp;
        }

        const AssetHandle& GetSceneHandle() { return m_SceneHandle; }

    private:
        uint32_t m_Width;
        uint32_t m_Height;
        glm::vec2 m_MousePosition;
        Scope<FrameBuffer> m_FrameBuffer;
        bool m_IsFocused;
        bool m_IsHovered;
        Entity& m_SelectedEntity;
        const Locale::Domain* m_GameDomain = nullptr;
        GuizmoOperation m_GuizmoOperation = GuizmoOperation::Translate;
        bool m_GuizmoSnap = false;

        mutable bool m_NewSceneWasLoaded = false;
        glm::vec2 m_ViewportBounds[2]{glm::vec2(0.0f), glm::vec2(0.0f)};
        Color4 m_ClearColor = Color4::CornflowerBlue;
        EditorAssetManager& m_AssetManager;
        Entity m_HoveredEntity = Entity::Null;
        Entity m_LastHoveredRuntime = Entity::Null;

        Path m_WorkingDirectory;

        AssetHandle m_SceneHandle;

        Ref<UniformBuffer> m_CameraUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4));
        Ref<BindingSet> m_CameraBindingSet = BindingSet::Create({{0, *m_CameraUniformBuffer}});

        bool OnMouseButtonPressed(MouseButtonPressedEvent* event) noexcept;
        bool OnKeyButtonPressed(KeyPressedEvent* event) noexcept;
        void RenderImGuizmo(EditorCamera& camera);
        void OpenScene(const Path& path);

        void RenderCameraFrustum(CommandBuffer& commandBuffer);

        void RenderSelectedEntityOutline(CommandBuffer& commandBuffer);

        void HandleReadPixelTask();

        bool IsMouseInViewport();
        Entity GetHoveredEntity();
    };
} // namespace BeeEngine::Editor
