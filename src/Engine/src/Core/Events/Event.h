#pragma once

#include <vector>
#include "Core/Logging/Log.h"

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
    protected:
        bool m_Handled = false;
    };

    class EventDispatcher
    {
    public:
        explicit EventDispatcher(Event& event)
        : m_event(event)
        {

        }
        inline EventCategory GetCategory() const
        {
            return m_event.Category;
        }

        template<class T>
        inline bool Dispatch(bool (*func) (T& event))
        {
            BeeCoreAssert(func, "Func is null");
            if(!dynamic_cast<T>(m_event))
                return false;
            if(func(m_event))
            {
                m_event.Handle();
            }
            return true;
        }
        inline bool IsHandled() const
        {
            return m_event.IsHandled();
        }
    private:
        Event& m_event;
    };
}