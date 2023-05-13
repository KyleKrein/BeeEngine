#pragma once
#include "Core/LayerStack.h"
namespace BeeEngine{
    class EventQueue
    {
    public:
        explicit EventQueue(LayerStack& layerStack);
        void AddEvent(Event* event);
        void Dispatch();
    private:
        void ApplicationOnEvent(EventDispatcher& e);

    private:
        std::vector<Event*> m_Events;
        LayerStack& m_LayerStack;
    };
}