#pragma once

#include "Core/Events/Event.h"

namespace BeeEngine
{
    class Layer
    {
    public:
        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate() {}
        virtual void OnGUIRendering() {}
        virtual void OnEvent(EventDispatcher& e) {}
    };

    class ImGuiLayer: public Layer
    {
    public:
        virtual void OnBegin() {};
        virtual void OnEnd() {};
    };
}