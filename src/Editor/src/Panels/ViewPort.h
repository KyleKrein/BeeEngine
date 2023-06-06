//
// Created by alexl on 26.05.2023.
//

#pragma once

#include "BeeEngine.h"
#include "Gui/ImGui/IImGuiElement.h"
#include "Scene/SceneCamera.h"

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
        ViewPort(uint32_t width, uint32_t height, Entity& selectedEntity) noexcept;
        void OnEvent(EventDispatcher& event) noexcept;
        void UpdateRuntime() noexcept;
        void UpdateEditor(EditorCamera& camera) noexcept;
        void Render(EditorCamera& camera) noexcept;
        Ref<Scene>& GetScene() noexcept { return m_Scene; }

        [[nodiscard]] uint32_t GetHeight() const
        {
            return m_Height;
        }
        [[nodiscard]] uint32_t GetWidth() const
        {
            return m_Width;
        }

    private:
        uint32_t m_Width;
        uint32_t m_Height;
        Scope<FrameBuffer> m_FrameBuffer;
        bool m_IsFocused;
        bool m_IsHovered;
        Ref<Scene> m_Scene;
        Entity& m_SelectedEntity;
        GuizmoOperation m_GuizmoOperation = GuizmoOperation::None;
        bool m_GuizmoSnap = false;


        bool OnMouseButtonPressed(MouseButtonPressedEvent* event) noexcept;
        void RenderImGuizmo(EditorCamera& camera);
    };
}
