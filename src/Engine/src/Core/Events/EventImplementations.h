//
// Created by alexl on 26.01.2023.
//

#pragma once
#include "Core/Events/Event.h"
#include "KeyCodes.h"

namespace BeeEngine
{
    struct WindowCloseEvent: public Event
    {
    public:
        WindowCloseEvent()
        {
            Category = EventCategory::App;
            m_Type = EventType::WindowClose;
        }
    };
    struct KeyPressedEvent: public Event
    {
    public:
        KeyPressedEvent(Key key)
        {
            Category = (EventCategory)(EventCategory::Keyboard & EventCategory::Input);
            m_Key = key;
            m_Type = EventType::KeyPressed;
        }
        Key GetKey()
        {
            return m_Key;
        }
    private:
        Key m_Key;
    };

    struct CharTypedEvent: public Event
    {
    public:
        CharTypedEvent(char character)
        {
            Category = (EventCategory)(EventCategory::Keyboard & EventCategory::Input);
            m_Character = character;
            m_Type = EventType::KeyTyped;
        }
        char GetCharacter()
        {
            return m_Character;
        }
    private:
        char m_Character;
    };

    struct MouseButtonPressedEvent: public Event
    {
    public:
        MouseButtonPressedEvent(MouseButton button)
        {
            Category = (EventCategory)(EventCategory::Mouse & EventCategory::Input);
            m_Button = button;
            m_Type = EventType::MouseButtonPressed;
        }
        MouseButton GetButton()
        {
            return m_Button;
        }
    private:
        MouseButton m_Button;
    };

    struct MouseMovedEvent: public Event
    {
    public:
        MouseMovedEvent(float x, float y)
        {
            Category = (EventCategory)(EventCategory::Mouse & EventCategory::Input);
            m_X = x;
            m_Y = y;
            m_Type = EventType::MouseMoved;
        }
        float GetX()
        {
            return m_X;
        }
        float GetY()
        {
            return m_Y;
        }
    private:
        float m_X, m_Y;
    };

    struct MouseScrolledEvent: public Event
    {
    public:
        MouseScrolledEvent(float xOffset, float yOffset)
        {
            Category = (EventCategory)(EventCategory::Mouse & EventCategory::Input);
            m_XOffset = xOffset;
            m_YOffset = yOffset;
            m_Type = EventType::MouseScrolled;
        }
        float GetXOffset()
        {
            return m_XOffset;
        }
        float GetYOffset()
        {
            return m_YOffset;
        }
    private:
        float m_XOffset, m_YOffset;
    };

    struct WindowResizeEvent: public Event
    {
    public:
        WindowResizeEvent(uint16_t width, uint16_t height)
        : m_Width(width), m_Height(height)
        {
            Category = EventCategory::App;
            m_Type = EventType::WindowResize;
        }
        uint16_t GetWidth()
        {
            return m_Width;
        }
        uint16_t GetHeight()
        {
            return m_Height;
        }
    private:
        uint16_t m_Width, m_Height;
    };
}