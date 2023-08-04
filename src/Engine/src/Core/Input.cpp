//
// Created by alexl on 26.01.2023.
//

#include "Input.h"
#include "Core/Events/EventImplementations.h"


namespace BeeEngine
{
    //bool Input::s_IsInit = false;
    std::unordered_map<Key, bool> Input::s_Keys
            {
                    {Key::Unknown, false},
                    {Key::Space, false},
                    {Key::Apostrophe, false},
                    {Key::Comma, false},
                    {Key::Minus, false},
                    {Key::Period, false},
                    {Key::Slash, false},
                    {Key::D0, false},
                    {Key::D1, false},
                    {Key::D2, false},
                    {Key::D3, false},
                    {Key::D4, false},
                    {Key::D5, false},
                    {Key::D6, false},
                    {Key::D7, false},
                    {Key::D8, false},
                    {Key::D9, false},
                    {Key::Semicolon, false},
                    {Key::Equal, false},
                    {Key::A, false},
                    {Key::B, false},
                    {Key::C, false},
                    {Key::D, false},
                    {Key::E, false},
                    {Key::F, false},
                    {Key::G, false},
                    {Key::H, false},
                    {Key::I, false},
                    {Key::J, false},
                    {Key::K, false},
                    {Key::L, false},
                    {Key::M, false},
                    {Key::N, false},
                    {Key::O, false},
                    {Key::P, false},
                    {Key::Q, false},
                    {Key::R, false},
                    {Key::S, false},
                    {Key::T, false},
                    {Key::U, false},
                    {Key::V, false},
                    {Key::W, false},
                    {Key::X, false},
                    {Key::Y, false},
                    {Key::Z, false},
                    {Key::LeftBracket, false},
                    {Key::Backslash, false},
                    {Key::RightBracket, false},
                    {Key::GraveAccent, false},
                    {Key::Escape, false},
                    {Key::Enter, false},
                    {Key::Tab, false},
                    {Key::Backspace, false},
                    {Key::Insert, false},
                    {Key::Delete, false},
                    {Key::Right, false},
                    {Key::Left, false},
                    {Key::Down, false},
                    {Key::Up, false},
                    {Key::PageUp, false},
                    {Key::PageDown, false},
                    {Key::Home, false},
                    {Key::End, false},
                    {Key::CapsLock, false},
                    {Key::ScrollLock, false},
                    {Key::NumLock, false},
                    {Key::PrintScreen, false},
                    {Key::Pause, false},
                    {Key::F1, false},
                    {Key::F2, false},
                    {Key::F3, false},
                    {Key::F4, false},
                    {Key::F5, false},
                    {Key::F6, false},
                    {Key::F7, false},
                    {Key::F8, false},
                    {Key::F9, false},
                    {Key::F10, false},
                    {Key::F11, false},
                    {Key::F12, false},
                    {Key::F13, false},
                    {Key::F14, false},
                    {Key::F15, false},
                    {Key::F16, false},
                    {Key::F17, false},
                    {Key::F18, false},
                    {Key::F19, false},
                    {Key::F20, false},
                    {Key::F21, false},
                    {Key::F22, false},
                    {Key::F23, false},
                    {Key::F24, false},
                    {Key::F25, false},
                    {Key::KeyPad0, false},
                    {Key::KeyPad1, false},
                    {Key::KeyPad2, false},
                    {Key::KeyPad3, false},
                    {Key::KeyPad4, false},
                    {Key::KeyPad5, false},
                    {Key::KeyPad6, false},
                    {Key::KeyPad7, false},
                    {Key::KeyPad8, false},
                    {Key::KeyPad9, false},
                    {Key::KeyPadDecimal, false},
                    {Key::KeyPadDivide, false},
                    {Key::KeyPadMultiply, false},
                    {Key::KeyPadSubtract, false},
                    {Key::KeyPadAdd, false},
                    {Key::KeyPadEnter, false},
                    {Key::KeyPadEqual, false},
                    {Key::LeftShift, false},
                    {Key::LeftControl, false},
                    {Key::LeftAlt, false},
                    {Key::LeftSuper, false},
                    {Key::RightShift, false},
                    {Key::RightControl, false},
                    {Key::RightAlt, false},
                    {Key::RightSuper, false},
                    {Key::Menu, false},
            };
    std::unordered_map<MouseButton, bool> Input::s_MouseButtons
            {
                    {MouseButton::Left, false},
                    {MouseButton::Right, false},
                    {MouseButton::Middle, false},
                    {MouseButton::Button4, false},
                    {MouseButton::Button5, false},
                    {MouseButton::Button6, false},
                    {MouseButton::Button7, false},
                    {MouseButton::Button8, false},
            };
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
                s_Keys[key] = true;
                break;
            }
            case BeeEngine::KeyReleased:
            {
                Key key = ((KeyReleasedEvent *) event)->GetKey();
                s_Keys[key] = false;
                break;
            }
            case BeeEngine::MouseButtonPressed:
            {
                MouseButton button = ((MouseButtonPressedEvent *) event)->GetButton();
                s_MouseButtons[button] = true;
                break;
            }
            case BeeEngine::MouseButtonReleased:
            {
                MouseButton button = ((MouseButtonReleasedEvent *) event)->GetButton();
                s_MouseButtons[button] = false;
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

    /*void Input::Init()
    {
        s_IsInit = true;
    }*/
}
