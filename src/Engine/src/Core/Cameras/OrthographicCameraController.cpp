//
// Created by alexl on 14.05.2023.
//

#include "OrthographicCameraController.h"
#include "Core/Input.h"

namespace BeeEngine
{

    OrthographicCameraController::OrthographicCameraController(uint32_t width, uint32_t height, bool rotation)
        : m_AspectRatio((float)width / (float)height),
        m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_CameraRotation(rotation),
        m_CameraPosition(0.0f, 0.0f, 0.0f),
        m_IsEnabled(true), m_ZoomStep(0.1f), m_ZoomLevel(1.0f)
    {

    }

    void OrthographicCameraController::OnUpdate()
    {
        BEE_PROFILE_FUNCTION();
        if (!m_IsEnabled)
            return;
        if(Input::KeyPressed(Key::W))
        {
            m_CameraPosition.y += m_MovementSpeed * Time::DeltaTime();
        }
        if(Input::KeyPressed(Key::S))
        {
            m_CameraPosition.y -= m_MovementSpeed * Time::DeltaTime();
        }
        if(Input::KeyPressed(Key::A))
        {
            m_CameraPosition.x -= m_MovementSpeed * Time::DeltaTime();
        }
        if(Input::KeyPressed(Key::D))
        {
            m_CameraPosition.x += m_MovementSpeed * Time::DeltaTime();
        }
        if (m_CameraRotation)
        {
            if (Input::KeyPressed(Key::Q))
            {
                m_Rotation -= m_RotationSpeed * Time::DeltaTime();
            }
            if (Input::KeyPressed(Key::E))
            {
                m_Rotation += m_RotationSpeed * Time::DeltaTime();
            }
            m_Camera.SetRotation(m_Rotation);
        }
        
        m_Camera.SetPosition(m_CameraPosition);
    }
    bool OrthographicCameraController::OnWindowResize(BeeEngine::WindowResizeEvent* event)
    {
        m_AspectRatio = (float)event->GetWidth() / (float)event->GetHeight();
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
        Renderer::SetViewport(0, 0, event->GetWidth(), event->GetHeight());
        return false;
    }
    void OrthographicCameraController::OnEvent(EventDispatcher& dispatcher)
    {

        DISPATCH_EVENT(dispatcher, WindowResizeEvent, EventType::WindowResize, OnWindowResize);

        if(!m_IsEnabled)
            return;

        DISPATCH_EVENT(dispatcher, MouseScrolledEvent, EventType::MouseScrolled, OnMouseScrolled);
    }

    bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent *event)
    {
        m_ZoomLevel -= event->GetYOffset() * m_ZoomStep;
        m_ZoomLevel = std::max(m_ZoomLevel, 0.1f);
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
        SetMovementSpeed(m_ZoomLevel);
        return false;
    }


}
