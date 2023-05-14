#pragma once

#include <utility>
#include <vector>
#include "Core/Logging/Log.h"
#include "Core/TypeDefines.h"
#include "ObjectPool.h"

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
        WindowLostFocus = 4,
        WindowMoved = 5,
        AppTick = 6,
        AppUpdate = 7,
        AppRender = 8,
        KeyPressed = 9,
        KeyTyped = 10,
        MouseButtonPressed = 11,
        MouseMoved = 12,
        MouseScrolled = 13
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
        //Events pool
        static void* operator new(size_t size);
        static void operator delete(void* ptr, size_t size) noexcept;
        static void* operator new[](size_t size);
        static void operator delete[](void* ptr, size_t size) noexcept;

        static void ClearPool()
        {
            s_EventPool.Clear();
        }

    private:
        static ObjectPool s_EventPool;
    };

    class EventDispatcher
    {
    public:
        explicit EventDispatcher(Event* event)
        : m_event(event)
        {

        }
        inline EventCategory GetCategory() const
        {
            return m_event->Category;
        }
        inline EventType GetType() const
        {
            return m_event->GetType();
        }

        template<class T, EventType T1>
        inline bool Dispatch(bool (*func) (T& event))
        {
            BeeCoreAssert(func, "Func is null");
            if(m_event->GetType() != T1)
                return false;
            if(func((T)m_event))
            {
                m_event->Handle();
            }
            return true;
        }
        inline bool IsHandled() const
        {
            return m_event->IsHandled();
        }
    private:
        Event* m_event;
    };
}