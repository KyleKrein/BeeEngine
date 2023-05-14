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
        explicit OrthographicCameraController(bool rotation = false): OrthographicCameraController(Application::GetInstance()->GetWidth()
                                                                                          , Application::GetInstance()->GetWidth(), rotation) {};
        OrthographicCameraController(uint32_t width, uint32_t  height, bool rotation = false);

        void OnUpdate();
        void OnEvent(EventDispatcher& dispatcher);

        void SetZoomLevel(float zoomLevel) { m_ZoomLevel = zoomLevel; }
        float GetZoomLevel() const { return m_ZoomLevel; }

        void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; }
        float GetAspectRatio() const { return m_AspectRatio; }

        void SetCameraPosition(const glm::vec3& position) { m_CameraPosition = position; }
        const glm::vec3& GetCameraPosition() const { return m_CameraPosition; }

        void SetCameraRotation(float rotation) { m_CameraRotation = rotation; }
        float GetCameraRotation() const { return m_CameraRotation; }
        OrthographicCamera& GetCamera() { return m_Camera; }

        float GetZoomStep() const { return m_ZoomStep; }
        void SetZoomStep(float zoomStep) { m_ZoomStep = zoomStep; }

        bool IsEnabled() const { return m_IsEnabled; }

        float GetMovementSpeed() const { return m_MovementSpeed; }
        void SetMovementSpeed(float movementSpeed) { m_MovementSpeed = movementSpeed; }

        float GetRotationSpeed() const { return m_RotationSpeed; }
        void SetRotationSpeed(float rotationSpeed) { m_RotationSpeed = rotationSpeed; }

        float GetRotation() const { return m_Rotation; }
        void SetRotation(float rotation) { m_Rotation = rotation; }
        operator OrthographicCamera& () { return m_Camera; }
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

        bool OnWindowResize(WindowResizeEvent* event);
        bool OnMouseScrolled(MouseScrolledEvent* event);
    };
}
