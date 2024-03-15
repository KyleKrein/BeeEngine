#pragma once

#include <gsl/gsl>
#include <utility>
#include <vector>
#include "Core/Logging/Log.h"
#include "Core/TypeDefines.h"
//#include "ObjectPool.h"

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
    enum EventType
    {
        WindowClose = 1,
        WindowResize = 2,
        WindowFocus = 3,
        WindowMoved = 4,
        WindowMinimized = 5,
        AppTick = 6,
        AppUpdate = 7,
        AppRender = 8,
        KeyPressed = 9,
        KeyReleased = 10,
        KeyTyped = 11,
        MouseButtonPressed = 12,
        MouseButtonReleased = 13,
        MouseMoved = 14,
        MouseScrolled = 15,
        FileDrop = 16,
        FileDrag = 17,
        FileDragEnter = 18,
        FileDragLeave = 19,
        FileDragStart = 20,
        FileDragEnd = 21,
    };
    class Event
    {
    public:
        EventCategory Category;
        inline void Handle()
        {
            m_Handled = true;
        }
        inline bool IsHandled() const
        {
            return m_Handled;
        }
        inline EventType GetType() const
        {
            return m_Type;
        }
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
        explicit EventDispatcher(gsl::not_null<Event*> event)
        : m_event(event)
        {

        }
        [[nodiscard]] inline EventCategory GetCategory() const
        {
            return m_event->Category;
        }
        [[nodiscard]] inline EventType GetType() const
        {
            return m_event->GetType();
        }

        template<typename EventType, typename Pred>
        requires std::is_base_of_v<Event, EventType> && std::is_invocable_r_v<bool, Pred, EventType&>
        inline bool Dispatch(Pred func)
        {
            if(m_event->GetType() != EventType::GetStaticType())
                return false;
            bool result = func(*static_cast<EventType*>(m_event));
            if(result)
            {
                m_event->Handle();
            }
            return true;
        }
        [[nodiscard]] inline bool IsHandled() const
        {
            return m_event->IsHandled();
        }
    private:
        Event* m_event;
    };
}