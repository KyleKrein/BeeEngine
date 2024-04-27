#pragma once
#include "Core/TypeDefines.h"
#include "Core/Events/Event.h"
#include "JobSystem/SpinLock.h"

namespace BeeEngine{
    struct LayerStack;

    class EventQueue
    {
    public:
        explicit EventQueue(LayerStack& layerStack);
        void AddEvent(Scope<Event>&& event);
        void Dispatch();
    private:
        void ApplicationOnEvent(EventDispatcher& e);

    private:
        std::vector<Scope<Event>> m_Events;
        LayerStack& m_LayerStack;
        Jobs::SpinLock m_Lock;
    };
}