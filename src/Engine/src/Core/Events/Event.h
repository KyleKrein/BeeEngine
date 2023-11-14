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
        AppTick = 5,
        AppUpdate = 6,
        AppRender = 7,
        KeyPressed = 8,
        KeyReleased = 9,
        KeyTyped = 10,
        MouseButtonPressed = 11,
        MouseButtonReleased = 12,
        MouseMoved = 13,
        MouseScrolled = 14,
        FileDrop = 15
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

        template<class T, EventType T1>
        inline bool Dispatch(bool (*func) (T& event))
        {
            BeeCoreAssert(func, "Func is null");
            if(m_event->GetType() != T1)
                return false;
            if(func(static_cast<T>(*m_event)))
            {
                m_event->Handle();
            }
            return true;
        }

        template<typename Event, EventType Type>
        bool Dispatch(std::function<bool(Event*)> func)
        {
            if(m_event->GetType() != Type)
                return false;
            bool result = func(static_cast<Event*>(m_event));
            if(result)
            {
                m_event->Handle();
            }
            return true;
        }


        /*template<class T, EventType T1>
        inline bool Dispatch(bool (*func) (T& event, void* ptr), void* ptr)
        {
            BeeCoreAssert(func, "Func is null");
            if(m_event->GetType() != T1)
                return false;
            if(func((T)m_event, ptr))
            {
                m_event->Handle();
            }
            return true;
        }*/
        [[nodiscard]] inline bool IsHandled() const
        {
            return m_event->IsHandled();
        }
    private:
        Event* m_event;
    };

#define DISPATCH_EVENT(dispatcher, Event, Type, Func) \
dispatcher.Dispatch<Event, Type>([this](Event* event) -> bool\
        {\
            return Func(reinterpret_cast<Event*>(event));\
        })
}