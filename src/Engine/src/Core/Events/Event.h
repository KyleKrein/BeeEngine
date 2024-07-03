#pragma once

#include <gsl/gsl>

namespace BeeEngine
{
    enum EventCategory
    {
        None = 0,
        App = 1,
        Input = 2,
        Keyboard = 4,
        Mouse = 8,
        MouseKey = 16
    };
    enum class EventType
    {
        WindowClose = 1,
        WindowResize = 2,
        WindowFocus = 3,
        WindowMoved = 4,
        WindowMinimized = 5,
        WindowMaximized = 6,
        AppTick = 7,
        AppUpdate = 8,
        AppRender = 9,
        KeyPressed = 10,
        KeyReleased = 11,
        KeyTyped = 12,
        MouseButtonPressed = 13,
        MouseButtonReleased = 14,
        MouseMoved = 15,
        MouseScrolled = 16,
        FileDrop = 17,
        FileDrag = 18,
        FileDragEnter = 19,
        FileDragLeave = 20,
        FileDragStart = 21,
        FileDragEnd = 22,
    };
    class Event
    {
    public:
        EventCategory Category;
        inline void Handle() { m_Handled = true; }
        inline bool IsHandled() const { return m_Handled; }
        inline EventType GetType() const { return m_Type; }

    protected:
        bool m_Handled = false;
        EventType m_Type;

    public:
        /*    //Events pool
            static void* operator new(size_t size);
            static void operator delete(void* ptr, size_t size) noexcept;
            static void* operator new[](size_t size);
            static void operator delete[](void* ptr, size_t size) noexcept;

            static void ClearPool()
            {
                s_EventPool.Clear();
            }

        private:
            static ObjectPool s_EventPool;*/
    };

    class EventDispatcher
    {
    public:
        explicit EventDispatcher(gsl::not_null<Event*> event) : m_event(event) {}
        [[nodiscard]] inline EventCategory GetCategory() const { return m_event->Category; }
        [[nodiscard]] inline EventType GetType() const { return m_event->GetType(); }

        template <typename EventType, typename Pred>
            requires std::is_base_of_v<Event, EventType> && std::is_invocable_r_v<bool, Pred, EventType&>
        inline bool Dispatch(Pred func)
        {
            if (m_event->GetType() != EventType::GetStaticType())
                return false;
            bool result = func(*static_cast<EventType*>(m_event));
            if (result)
            {
                m_event->Handle();
            }
            return true;
        }
        [[nodiscard]] inline bool IsHandled() const { return m_event->IsHandled(); }

    private:
        Event* m_event;
    };
} // namespace BeeEngine