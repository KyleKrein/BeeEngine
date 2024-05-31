//
// Created by alexl on 26.01.2023.
//

#pragma once
#include "Core/Events/Event.h"
#include "Core/Path.h"
#include "KeyCodes.h"
#include "Windowing/WindowHandler/WindowHandler.h"

namespace BeeEngine
{
    struct WindowCloseEvent : public Event
    {
    public:
        static consteval EventType GetStaticType() noexcept { return EventType::WindowClose; }
        WindowCloseEvent() noexcept
        {
            Category = EventCategory::App;
            m_Type = EventType::WindowClose;
        }
    };
    struct KeyPressedEvent : public Event
    {
    public:
        static consteval EventType GetStaticType() noexcept { return EventType::KeyPressed; }
        explicit KeyPressedEvent(Key key, int repeat = 0) noexcept
        {
            Category = static_cast<EventCategory>(EventCategory::Keyboard | EventCategory::Input);
            m_Key = key;
            m_Repeat = repeat;
            m_Type = EventType::KeyPressed;
        }
        Key GetKey() { return m_Key; }

    private:
        Key m_Key;
        int m_Repeat;
    };
    struct KeyReleasedEvent : public Event
    {
    public:
        static consteval EventType GetStaticType() noexcept { return EventType::KeyReleased; }
        explicit KeyReleasedEvent(Key key) noexcept
        {
            Category = static_cast<EventCategory>(EventCategory::Keyboard & EventCategory::Input);
            m_Key = key;
            m_Type = EventType::KeyReleased;
        }
        Key GetKey() { return m_Key; }

    private:
        Key m_Key;
    };

    struct CharTypedEvent : public Event
    {
    public:
        static consteval EventType GetStaticType() noexcept { return EventType::KeyTyped; }
        explicit CharTypedEvent(char32_t character) noexcept
        {
            Category = static_cast<EventCategory>(EventCategory::Keyboard & EventCategory::Input);
            m_Character = character;
            m_Type = EventType::KeyTyped;
        }
        [[nodiscard]] char32_t GetCharacter() const { return m_Character; }

    private:
        char32_t m_Character;
    };

    struct MouseButtonPressedEvent : public Event
    {
    public:
        static consteval EventType GetStaticType() noexcept { return EventType::MouseButtonPressed; }
        explicit MouseButtonPressedEvent(MouseButton button) noexcept
        {
            Category = static_cast<EventCategory>(EventCategory::Mouse & EventCategory::Input);
            m_Button = button;
            m_Type = EventType::MouseButtonPressed;
        }
        MouseButton GetButton() { return m_Button; }

    private:
        MouseButton m_Button;
    };

    struct MouseButtonReleasedEvent : public Event
    {
    public:
        static consteval EventType GetStaticType() noexcept { return EventType::MouseButtonReleased; }
        explicit MouseButtonReleasedEvent(MouseButton button) noexcept : m_Button(button)
        {
            Category = static_cast<EventCategory>(EventCategory::Mouse & EventCategory::Input);
            m_Type = EventType::MouseButtonReleased;
        }
        MouseButton GetButton() { return m_Button; }

    private:
        MouseButton m_Button;
    };

    struct MouseMovedEvent : public Event
    {
    public:
        static consteval EventType GetStaticType() noexcept { return EventType::MouseMoved; }
        MouseMovedEvent(float x, float y) noexcept
        {
            Category = static_cast<EventCategory>(EventCategory::Mouse & EventCategory::Input);
            m_X = x;
            m_Y = y;
            m_Type = EventType::MouseMoved;
        }
        [[nodiscard]] float GetX() const { return m_X; }
        [[nodiscard]] float GetY() const { return m_Y; }

    private:
        float m_X, m_Y;
    };

    struct MouseScrolledEvent : public Event
    {
    public:
        static consteval EventType GetStaticType() noexcept { return EventType::MouseScrolled; }
        MouseScrolledEvent(float xOffset, float yOffset) noexcept
        {
            Category = static_cast<EventCategory>(EventCategory::Mouse & EventCategory::Input);
            m_XOffset = xOffset;
            m_YOffset = yOffset;
            m_Type = EventType::MouseScrolled;
        }
        [[nodiscard]] float GetXOffset() const { return m_XOffset; }
        [[nodiscard]] float GetYOffset() const { return m_YOffset; }

    private:
        float m_XOffset, m_YOffset;
    };

    struct WindowResizeEvent : public Event
    {
    public:
        static consteval EventType GetStaticType() noexcept { return EventType::WindowResize; }
        WindowResizeEvent(uint16_t width, uint16_t height, uint16_t pixelwidth, uint16_t pixelheight) noexcept
            : m_Width(width), m_Height(height), m_WidthInPixels(pixelwidth), m_HeightInPixels(pixelheight)
        {
            Category = EventCategory::App;
            m_Type = EventType::WindowResize;
        }
        [[nodiscard]] inline uint16_t GetWidthInPoints() const { return m_Width; }
        [[nodiscard]] inline uint16_t GetHeightInPoints() const { return m_Height; }
        [[nodiscard]] inline uint16_t GetWidthInPixels() const { return m_WidthInPixels; }
        [[nodiscard]] inline uint16_t GetHeightInPixels() const { return m_HeightInPixels; }

    private:
        uint16_t m_Width, m_Height, m_WidthInPixels, m_HeightInPixels;
    };

    struct WindowFocusedEvent : public Event
    {
    public:
        static consteval EventType GetStaticType() noexcept { return EventType::WindowFocus; }
        WindowFocusedEvent(bool isFocused) noexcept : m_IsFocused(isFocused)
        {
            Category = EventCategory::App;
            m_Type = EventType::WindowFocus;
        }
        [[nodiscard]] bool IsFocused() const { return m_IsFocused; }

    private:
        bool m_IsFocused;
    };
    namespace Internal
    {
        class SDLWindowHandler;
#if defined(WINDOWS)
        class WindowsDropTarget;
#endif
    } // namespace Internal
    /*
     * Fires when user drops file(s) on the window.
     */
    struct FileDropEvent : public Event
    {
        friend Internal::SDLWindowHandler;
        friend void __internal_bee_add_file_to_drop_event(FileDropEvent* event, String&& path);
#if defined(WINDOWS)
        friend Internal::WindowsDropTarget;
#endif
    public:
        static consteval EventType GetStaticType() noexcept { return EventType::FileDrop; }

        FileDropEvent()
        {
            Category = EventCategory::App;
            m_Type = EventType::FileDrop;
        }
        [[nodiscard]] const std::vector<Path>& GetPaths() const { return m_Paths; }
        [[nodiscard]] std::vector<Path>&& GetPathsMove() noexcept { return std::move(m_Paths); }

    private:
        void AddFile(const char* path) { m_Paths.emplace_back(path); }
        void AddFile(String&& path) { m_Paths.emplace_back(std::move(path)); }
        std::vector<Path> m_Paths;
    };
    /*
     * Fires when mouse was over the window and then moved over it
     * while dragging something from outside of the app.
     * Format of coordinates is not standardised. It's platform specific.
     */
    struct FileDragEvent : public Event
    {
    public:
        static consteval EventType GetStaticType() noexcept { return EventType::FileDrag; }
        FileDragEvent(int32_t x, int32_t y) : m_X(x), m_Y(y)
        {
            Category = EventCategory::App;
            m_Type = EventType::FileDrag;
        }
        [[nodiscard]] int32_t GetX() const { return m_X; }
        [[nodiscard]] int32_t GetY() const { return m_Y; }
        // In Local to window coords
        bool IsInRect(int32_t x, int32_t y, int32_t width, int32_t height) const
        {
            return m_X >= x && m_X <= x + width && m_Y >= y && m_Y <= y + height;
        }
        bool IsInAppWindow() const
        {
            return m_X >= 0 && m_X <= WindowHandler::GetInstance()->GetWidth() && m_Y >= 0 &&
                   m_Y <= WindowHandler::GetInstance()->GetHeight();
        }

    private:
        int32_t m_X, m_Y;
    };
    /*
     * Fires when mouse was outside of the window and then moved over it
     * while dragging something from outside of the app.
     * Format of coordinates is not standardised. It's platform specific.
     */
    struct FileDragEnterEvent : public Event
    {
    public:
        static consteval EventType GetStaticType() noexcept { return EventType::FileDragEnter; }
        FileDragEnterEvent(int32_t x, int32_t y) : m_X(x), m_Y(y)
        {
            Category = EventCategory::App;
            m_Type = EventType::FileDragEnter;
        }
        [[nodiscard]] int32_t GetX() const { return m_X; }
        [[nodiscard]] int32_t GetY() const { return m_Y; }

    private:
        int32_t m_X, m_Y;
    };
    /*
     * Fires when mouse was over the window and then moved out of it
     * while dragging something from outside of the app.
     * Format of coordinates is not standardised. It's platform specific.
     */
    struct FileDragLeaveEvent : public Event
    {
    public:
        static consteval EventType GetStaticType() noexcept { return EventType::FileDragLeave; }
        FileDragLeaveEvent(int32_t x, int32_t y) : m_X(x), m_Y(y)
        {
            Category = EventCategory::App;
            m_Type = EventType::FileDragLeave;
        }
        [[nodiscard]] int32_t GetX() const { return m_X; }
        [[nodiscard]] int32_t GetY() const { return m_Y; }

    private:
        int32_t m_X, m_Y;
    };
    /* Fires before any other DragAndDrop event.
     * When fired, it means that User is dragging something from outside of the app.
     * This event is fired only once per drag and drop session.
     */
    struct FileDragStartEvent : public Event
    {
    public:
        static consteval EventType GetStaticType() noexcept { return EventType::FileDragStart; }
        FileDragStartEvent()
        {
            Category = EventCategory::App;
            m_Type = EventType::FileDragStart;
        }

    private:
    };
    /*
     * Fires after FileDragStartEvent at the end of drag and drop session.
     * This event is fired only once per drag and drop session.
     * It means that one way or another drag and drop session has ended.
     * And no more FileDragEnterEvent, FileDragEvent, FileDragLeaveEvent will be fired
     * until the next FileDragStartEvent.
     */
    struct FileDragEndEvent : public Event
    {
    public:
        static consteval EventType GetStaticType() noexcept { return EventType::FileDragEnd; }
        FileDragEndEvent()
        {
            Category = EventCategory::App;
            m_Type = EventType::FileDragEnd;
        }
    };
    struct WindowMovedEvent : public Event
    {
    public:
        static consteval EventType GetStaticType() noexcept { return EventType::WindowMoved; }
        WindowMovedEvent(int32_t x, int32_t y) : m_X(x), m_Y(y)
        {
            Category = EventCategory::App;
            m_Type = EventType::WindowMoved;
        }
        [[nodiscard]] int32_t GetX() const { return m_X; }
        [[nodiscard]] int32_t GetY() const { return m_Y; }

    private:
        int32_t m_X, m_Y;
    };

    struct WindowMinimizedEvent : public Event
    {
    public:
        static consteval EventType GetStaticType() noexcept { return EventType::WindowMinimized; }
        WindowMinimizedEvent(bool isMinimized) : m_IsMinimized(isMinimized)
        {
            Category = EventCategory::App;
            m_Type = EventType::WindowMinimized;
        }
        [[nodiscard]] bool IsMinimized() const { return m_IsMinimized; }

    private:
        const bool m_IsMinimized;
    };
} // namespace BeeEngine