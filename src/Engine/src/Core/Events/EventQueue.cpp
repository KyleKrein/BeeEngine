//
// Created by alexl on 26.01.2023.
//

#include "EventQueue.h"
#include "Core/Application.h"
#include "Core/Input.h"
namespace BeeEngine
{
    EventQueue::EventQueue(LayerStack& layerStack)
    :m_Events(), m_LayerStack(layerStack)
    {

    }

    void EventQueue::AddEvent(Event& event)
    {
        m_Events.push_back(&event);
    }
    void EventQueue::AddEvent(Event *event)
    {
        m_Events.push_back(event);
    }

    void EventQueue::Dispatch()
    {
        for (int i = 0; i < m_Events.size(); ++i)
        {
            Event* event = m_Events[i];
            EventDispatcher dispatcher(event);
            ApplicationOnEvent(dispatcher);
            Input::OnEvent(event);
            m_LayerStack.OnEvent(dispatcher);
            delete event;
        }
        m_Events.clear();
    }

    void EventQueue::ApplicationOnEvent(EventDispatcher &e)
    {
        Application::s_Instance->Dispatch(e);
    }
}
