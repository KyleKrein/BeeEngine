//
// Created by alexl on 06.06.2023.
//

#pragma once

#include "Core/Cameras/Camera.h"
#include "Core/Events/Event.h"
#include "Core/Events/EventImplementations.h"

namespace BeeEngine
{
    class EditorCamera: public Camera
    {
        public:
        EditorCamera() = default;
        EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

        void OnUpdate();
        void OnEvent(EventDispatcher& dispatcher);

        inline float GetDistance() const noexcept { return m_Distance; }
        inline void SetDistance(float distance) noexcept { m_Distance = distance; }

        inline void SetViewportSize(float width, float height) noexcept
        {
            m_ViewportWidth = width;
            m_ViewportHeight = height;
            UpdateProjection();
        }

        const glm::mat4& GetViewMatrix() const noexcept { return m_ViewMatrix; }

        glm::vec3 GetUpDirection() const noexcept;
        glm::vec3 GetRightDirection() const noexcept;
        glm::vec3 GetForwardDirection() const noexcept;
        const glm::vec3& GetPosition() const noexcept { return m_Position; }
        glm::quat GetOrientation() const noexcept;

        float GetPitch() const noexcept { return m_Pitch; }
        float GetYaw() const noexcept { return m_Yaw; }
        glm::mat4 GetViewProjection() const noexcept { return m_ProjectionMatrix * m_ViewMatrix; }

    private:
        void UpdateProjection() noexcept;
        void UpdateView() noexcept;

        bool OnMouseScrolled(MouseScrolledEvent* event);

        void MousePan(const glm::vec2& delta);
        void MouseRotate(const glm::vec2& delta);
        void MouseZoom(float delta);

        glm::vec3 CalculatePosition() const noexcept;

        std::pair<float, float> PanSpeed() const noexcept;
        float RotationSpeed() const noexcept;
        float ZoomSpeed() const noexcept;

    private:
        float m_Fov = 45.0f;
        float m_AspectRatio = 1.778f;
        float m_NearClip = 0.1f;
        float m_FarClip = 1000.0f;

        glm::mat4 m_ViewMatrix{1.0f};
        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

        glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

        float m_Distance = 10.0f;
        float m_Pitch = 0.0f;
        float m_Yaw = 0.0f;

        float m_ViewportWidth = 1280;
        float m_ViewportHeight = 720;
    };
}
