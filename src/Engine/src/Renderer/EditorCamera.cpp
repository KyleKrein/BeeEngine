//
// Created by alexl on 06.06.2023.
//

#include "EditorCamera.h"
#include "Core/Input.h"
#include "Debug/Instrumentor.h"
#include <glm/detail/type_quat.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/quaternion.hpp>

namespace BeeEngine
{

    EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
        : m_Fov(fov),
          m_AspectRatio(aspectRatio),
          m_NearClip(nearClip),
          m_FarClip(farClip),
          Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip)),
          m_ViewMatrix(1.0f)
    {
        UpdateView();
    }

    void EditorCamera::OnUpdate()
    {
        if (Input::KeyPressed(Key::LeftAlt))
        {
            const glm::vec2& mouse{Input::GetMouseX(), Input::GetMouseY()};
            glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
            m_InitialMousePosition = mouse;

            if (Input::MouseKeyPressed(MouseButton::Middle))
                MousePan(delta);
            else if (Input::MouseKeyPressed(MouseButton::Left))
                MouseRotate(delta);
            else if (Input::MouseKeyPressed(MouseButton::Right))
                MouseZoom(delta.y);
        }

        UpdateView();
    }

    void EditorCamera::OnEvent(EventDispatcher& dispatcher)
    {
        dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& event) -> bool
                                                { return OnMouseScrolled(&event); });
    }

    glm::vec3 EditorCamera::GetUpDirection() const noexcept
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::vec3 EditorCamera::GetRightDirection() const noexcept
    {
        return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::vec3 EditorCamera::GetForwardDirection() const noexcept
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::quat EditorCamera::GetOrientation() const noexcept
    {
        return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
    }

    void EditorCamera::UpdateProjection() noexcept
    {
        m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
        m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearClip, m_FarClip);
    }

    void EditorCamera::UpdateView() noexcept
    {
        // m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
        m_Position = CalculatePosition();

        glm::quat orientation = GetOrientation();
        m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
        // m_ViewMatrix = glm::inverse(m_ViewMatrix);
    }

    bool EditorCamera::OnMouseScrolled(MouseScrolledEvent* event)
    {
        float delta = event->GetYOffset() * 0.1f;
        MouseZoom(delta);
        UpdateView();
        return false;
    }

    void EditorCamera::MousePan(const glm::vec2& delta)
    {
        auto [xSpeed, ySpeed] = PanSpeed();
        m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
        m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
    }

    void EditorCamera::MouseRotate(const glm::vec2& delta)
    {
        float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
        m_Yaw += yawSign * delta.x * RotationSpeed();
        m_Pitch += delta.y * RotationSpeed();
    }

    void EditorCamera::MouseZoom(float delta)
    {
        m_Distance -= delta * ZoomSpeed();
        if (m_Distance < 1.0f)
        {
            m_FocalPoint += GetForwardDirection();
            m_Distance = 1.0f;
        }
    }

    glm::vec3 EditorCamera::CalculatePosition() const noexcept
    {
        return m_FocalPoint - GetForwardDirection() * m_Distance;
    }

    std::pair<float, float> EditorCamera::PanSpeed() const noexcept
    {
        float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
        float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

        float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
        float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

        return {xFactor, yFactor};
    }

    float EditorCamera::RotationSpeed() const noexcept
    {
        return 0.8f;
    }

    float EditorCamera::ZoomSpeed() const noexcept
    {
        float distance = m_Distance * 0.2f;
        distance = std::max(distance, 0.0f);
        float speed = distance * distance;
        speed = std::min(speed, 100.0f); // max speed = 100
        return speed;
    }
} // namespace BeeEngine
