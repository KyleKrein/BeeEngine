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

    void EventQueue::AddEvent(Scope<Event>&& event)
    {
        BEE_PROFILE_FUNCTION();
        m_Events.push_back(std::move(event));
    }

    void EventQueue::Dispatch()
    {
        BEE_PROFILE_FUNCTION();
        for (int i = 0; i < m_Events.size(); ++i)
        {
            Event* event = m_Events[i].get();
            EventDispatcher dispatcher(event);
            ApplicationOnEvent(dispatcher);
            Input::OnEvent(event);
            m_LayerStack.OnEvent(dispatcher);
        }
        //Event::ClearPool();
        m_Events.clear();
    }

    void EventQueue::ApplicationOnEvent(EventDispatcher &e)
    {
        BEE_PROFILE_FUNCTION();
        Application::s_Instance->Dispatch(e);
    }
}

