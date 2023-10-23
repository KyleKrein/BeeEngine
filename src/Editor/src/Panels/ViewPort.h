//
// Created by alexl on 26.05.2023.
//

#pragma once

#include "BeeEngine.h"
#include "Gui/ImGui/IImGuiElement.h"
#include "Scene/SceneCamera.h"
#include "Scene/Entity.h"
#include <ImGuizmo.h>
#include "Locale/Locale.h"

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
        ViewPort(uint32_t width, uint32_t height, Entity& selectedEntity, const Color4& clearColor = Color4::CornflowerBlue) noexcept;
        void OnEvent(EventDispatcher& event) noexcept;
        void UpdateRuntime() noexcept;
        void UpdateEditor(EditorCamera& camera) noexcept;
        void Render(EditorCamera& camera) noexcept;
        Ref<Scene>& GetScene() noexcept { return m_Scene; }
        void SetScene(const Ref<Scene>& scene) noexcept { m_Scene.reset(); m_Scene = scene; }
        void SetDomain(const Locale::Domain* domain) noexcept { m_GameDomain = domain; }

        [[nodiscard]] uint32_t GetHeight() const
        {
            return m_Height;
        }
        [[nodiscard]] uint32_t GetWidth() const
        {
            return m_Width;
        }

        void SetWorkingDirectory(const Path& path) noexcept
        {
            m_WorkingDirectory = path;
        }

        bool IsNewSceneLoaded() const noexcept
        {
            bool tmp = m_NewSceneWasLoaded;
            m_NewSceneWasLoaded = false;
            return tmp;
        }

        const std::string& GetScenePath()
        {
            return m_ScenePath;
        }

    private:
        uint32_t m_Width;
        uint32_t m_Height;
        Scope<FrameBuffer> m_FrameBuffer;
        bool m_IsFocused;
        bool m_IsHovered;
        Ref<Scene> m_Scene;
        Entity& m_SelectedEntity;
        const Locale::Domain* m_GameDomain = nullptr;
        GuizmoOperation m_GuizmoOperation = GuizmoOperation::Translate;
        bool m_GuizmoSnap = false;

        mutable bool m_NewSceneWasLoaded = false;
        glm::vec2 m_ViewportBounds[2]
                {
                        glm::vec2(0.0f),
                        glm::vec2(0.0f)
                };
        Color4 m_ClearColor = Color4::CornflowerBlue;
        Entity m_HoveredEntity = Entity::Null;

        Path m_WorkingDirectory;

        std::string m_ScenePath;


        bool OnMouseButtonPressed(MouseButtonPressedEvent* event) noexcept;
        bool OnKeyButtonPressed(KeyPressedEvent* event) noexcept;
        void RenderImGuizmo(EditorCamera& camera);
        void OpenScene(const Path& path);
    };
}
