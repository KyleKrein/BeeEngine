//
// Created by alexl on 26.01.2023.
//

#pragma once
#include "Core/Events/Event.h"
#include "KeyCodes.h"
#include "Core/Path.h"

namespace BeeEngine
{
    struct WindowCloseEvent: public Event
    {
    public:
        WindowCloseEvent() noexcept
        {
            Category = EventCategory::App;
            m_Type = EventType::WindowClose;
        }
    };
    struct KeyPressedEvent: public Event
    {
    public:
        explicit KeyPressedEvent(Key key, int repeat = 0) noexcept
        {
            Category = static_cast<EventCategory>(EventCategory::Keyboard | EventCategory::Input);
            m_Key = key;
            m_Repeat = repeat;
            m_Type = EventType::KeyPressed;
        }
        Key GetKey()
        {
            return m_Key;
        }
    private:
        Key m_Key;
        int m_Repeat;
    };
    struct KeyReleasedEvent: public Event
    {
    public:
        explicit KeyReleasedEvent(Key key) noexcept
        {
            Category = static_cast<EventCategory>(EventCategory::Keyboard & EventCategory::Input);
            m_Key = key;
            m_Type = EventType::KeyReleased;
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
        explicit CharTypedEvent(char32_t character) noexcept
        {
            Category = static_cast<EventCategory>(EventCategory::Keyboard & EventCategory::Input);
            m_Character = character;
            m_Type = EventType::KeyTyped;
        }
        [[nodiscard]] char32_t GetCharacter() const
        {
            return m_Character;
        }
    private:
        char32_t m_Character;
    };

    struct MouseButtonPressedEvent: public Event
    {
    public:
        explicit MouseButtonPressedEvent(MouseButton button) noexcept
        {
            Category = static_cast<EventCategory>(EventCategory::Mouse & EventCategory::Input);
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

    struct MouseButtonReleasedEvent: public Event
    {
    public:
        explicit MouseButtonReleasedEvent(MouseButton button) noexcept
        : m_Button(button)
        {
            Category = static_cast<EventCategory>(EventCategory::Mouse & EventCategory::Input);
            m_Type = EventType::MouseButtonReleased;
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
        MouseMovedEvent(float x, float y) noexcept
        {
            Category = static_cast<EventCategory>(EventCategory::Mouse & EventCategory::Input);
            m_X = x;
            m_Y = y;
            m_Type = EventType::MouseMoved;
        }
        [[nodiscard]] float GetX() const
        {
            return m_X;
        }
        [[nodiscard]] float GetY() const
        {
            return m_Y;
        }
    private:
        float m_X, m_Y;
    };

    struct MouseScrolledEvent: public Event
    {
    public:
        MouseScrolledEvent(float xOffset, float yOffset) noexcept
        {
            Category = static_cast<EventCategory>(EventCategory::Mouse & EventCategory::Input);
            m_XOffset = xOffset;
            m_YOffset = yOffset;
            m_Type = EventType::MouseScrolled;
        }
        [[nodiscard]] float GetXOffset() const
        {
            return m_XOffset;
        }
        [[nodiscard]] float GetYOffset() const
        {
            return m_YOffset;
        }
    private:
        float m_XOffset, m_YOffset;
    };

    struct WindowResizeEvent: public Event
    {
    public:
        WindowResizeEvent(uint16_t width, uint16_t height) noexcept
        : m_Width(width), m_Height(height)
        {
            Category = EventCategory::App;
            m_Type = EventType::WindowResize;
        }
        [[nodiscard]] uint16_t GetWidth() const
        {
            return m_Width;
        }
        [[nodiscard]] uint16_t GetHeight() const
        {
            return m_Height;
        }
    private:
        uint16_t m_Width, m_Height;
    };

    struct WindowFocusedEvent: public Event
    {
    public:
        WindowFocusedEvent(bool isFocused) noexcept
        : m_IsFocused(isFocused)
        {
            Category = EventCategory::App;
            m_Type = EventType::WindowFocus;
        }
        bool IsFocused() const
        {
            return m_IsFocused;
        }
    private:
        bool m_IsFocused;
    };

    struct FileDropEvent: public Event
    {
    public:
        FileDropEvent(Path&& path)
        : m_Path(std::move(path))
        {
            Category = EventCategory::App;
            m_Type = EventType::FileDrop;
        }
        [[nodiscard]] const Path& GetPath() const
        {
            return m_Path;
        }
    private:
        Path m_Path;
    };
}