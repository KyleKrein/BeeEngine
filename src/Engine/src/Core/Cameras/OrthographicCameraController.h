//
// Created by alexl on 14.05.2023.
//

#pragma once

#include "vec3.hpp"
#include "fwd.hpp"
#include "OrthographicCamera.h"
#include "Core/Application.h"

namespace BeeEngine
{
    class OrthographicCameraController
    {
    public:
        explicit OrthographicCameraController(bool rotation = false) noexcept: OrthographicCameraController(Application::GetInstance()->GetWidth()
                                                                                          , Application::GetInstance()->GetWidth(), rotation) {};
        OrthographicCameraController(uint32_t width, uint32_t  height, bool rotation = false) noexcept;

        void OnUpdate() noexcept;
        void OnEvent(EventDispatcher& dispatcher) noexcept;

        inline void SetZoomLevel(float zoomLevel) noexcept { m_ZoomLevel = zoomLevel; }
        [[nodiscard]] inline float GetZoomLevel() const noexcept { return m_ZoomLevel; }

        inline void SetAspectRatio(float aspectRatio) noexcept { m_AspectRatio = aspectRatio; }
        [[nodiscard]] inline float GetAspectRatio() const noexcept { return m_AspectRatio; }

        inline void SetCameraPosition(const glm::vec3& position) noexcept { m_CameraPosition = position; }
        [[nodiscard]]inline const glm::vec3& GetCameraPosition() const noexcept { return m_CameraPosition; }

        inline void SetCameraRotation(float rotation) noexcept { m_CameraRotation = rotation; }
        [[nodiscard]] inline float GetCameraRotation() const noexcept { return m_CameraRotation; }
        inline OrthographicCamera& GetCamera() noexcept { return m_Camera; }

        [[nodiscard]] inline float GetZoomStep() const noexcept { return m_ZoomStep; }
        inline void SetZoomStep(float zoomStep) noexcept { m_ZoomStep = zoomStep; }

        [[nodiscard]] inline bool IsEnabled() const noexcept { return m_IsEnabled; }

        [[nodiscard]] inline float GetMovementSpeed() const noexcept { return m_MovementSpeed; }
        inline void SetMovementSpeed(float movementSpeed) noexcept { m_MovementSpeed = movementSpeed; }

        [[nodiscard]] inline float GetRotationSpeed() const noexcept { return m_RotationSpeed; }
        inline void SetRotationSpeed(float rotationSpeed) noexcept { m_RotationSpeed = rotationSpeed; }

        [[nodiscard]] inline float GetRotation() const noexcept { return m_Rotation; }
        inline void SetRotation(float rotation) noexcept { m_Rotation = rotation; }
        inline operator OrthographicCamera& () noexcept { return m_Camera; }

        inline void Disable() noexcept
        {
            m_IsEnabled = false;
        }

        inline void Enable() noexcept
        {
            m_IsEnabled = true;
        }

    private:

        float m_ZoomLevel = 1.0f;
        float m_AspectRatio;
        bool m_CameraRotation;
        glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
        float m_ZoomStep = 0.1f;
        bool m_IsEnabled = true;
        OrthographicCamera m_Camera;
        float m_MovementSpeed = 1;
        float m_RotationSpeed = 90;
        float m_Rotation = 0.0f;

        inline bool OnWindowResize(WindowResizeEvent* event) noexcept;
        inline bool OnMouseScrolled(MouseScrolledEvent* event) noexcept;
    };
}
