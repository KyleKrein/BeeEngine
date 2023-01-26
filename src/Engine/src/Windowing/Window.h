#pragma once
#include "Core/TypeDefines.h"
#include "WindowProperties.h"
#include "Core/Events/EventQueue.h"

namespace BeeEngine{
    class Window
    {
    public:
        virtual ~Window() {};
        static Window* Create(const WindowProperties& properties,EventQueue& eventQueue);
        virtual uint16_t GetWidth() const = 0;
        virtual uint16_t GetHeight() const = 0;
        virtual void SetWidth(uint16_t width) = 0;
        virtual void SetHeight(uint16_t height) = 0;
    };
}
