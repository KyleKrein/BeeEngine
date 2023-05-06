//
// Created by alexl on 26.01.2023.
//

#include "Input.h"
#include "Core/Events/EventImplementations.h"


namespace BeeEngine
{
    bool Input::s_IsInit = false;
    std::unordered_map<Key, bool> Input::s_Keys;
    std::unordered_map<MouseButton, bool> Input::s_MouseButtons;
    float Input::s_MouseX = 0.0f;
    float Input::s_MouseY = 0.0f;
    float Input::s_MouseWheelX = 0.0f;
    float Input::s_MouseWheelY = 0.0f;

    bool Input::KeyPressed(Key key)
    {
        return s_Keys[key];
    }

    bool Input::MouseKeyPressed(MouseButton button)
    {
        return s_MouseButtons[button];
    }

    void Input::OnEvent(Event* event)
    {
        switch (event->GetType())
        {
            case BeeEngine::KeyPressed:
            {
                Key key = ((KeyPressedEvent *) event)->GetKey();
                s_Keys[key] = !s_Keys[key];
                break;
            }
            case BeeEngine::MouseButtonPressed:
            {
                MouseButton button = ((MouseButtonPressedEvent *) event)->GetButton();
                s_MouseButtons[button] = !s_MouseButtons[button];
                break;
            }
            case BeeEngine::MouseMoved:
            {
                s_MouseX = ((MouseMovedEvent *) event)->GetX();
                s_MouseY = ((MouseMovedEvent *) event)->GetY();
                break;
            }
            case BeeEngine::MouseScrolled:
            {
                s_MouseWheelX = ((MouseScrolledEvent *) event)->GetXOffset();
                s_MouseWheelY = ((MouseScrolledEvent *) event)->GetYOffset();
                break;
            }
            default:
                break;
        }
    }

    void Input::Init()
    {
        s_IsInit = true;
    }
}
