#pragma once

#include "Core/TypeDefines.h"
#include "Windowing/WindowProperties.h"
#include "Core/Events/EventQueue.h"

namespace BeeEngine
{
    enum class WindowHandlerAPI
    {
        GLFW = 0,
        SDL = 1
    };
    class WindowHandler
    {
    public:
        virtual ~WindowHandler() = default;
        static WindowHandler* Create(WindowHandlerAPI api, const WindowProperties& properties, EventQueue& eventQueue);
        uint16_t GetWidth() const
        {
            return m_Width;
        }
        uint16_t GetHeight() const
        {
            return m_Height;
        }
        virtual void SetWidth(uint16_t width) = 0;
        virtual void SetHeight(uint16_t height) = 0;
        VSync GetVSync() const
        {
            return m_vsync;
        }
        virtual void SetVSync(VSync mode) = 0;

        virtual void HideCursor() = 0;
        virtual void DisableCursor() = 0;
        virtual void ShowCursor() = 0;
        virtual void ProcessEvents() = 0;
        virtual void SwapBuffers() = 0;
        virtual void MakeContextCurrent() = 0;
        virtual void MakeContextNonCurrent() = 0;
        virtual bool IsRunning() const = 0;
    protected:
        static WindowHandler* s_Instance;
        WindowHandler() = delete;
        WindowHandler(EventQueue& eventQueue): m_Events(eventQueue){};
        uint16_t m_Width;
        uint16_t m_Height;
        const char *m_Title;
        VSync m_vsync;
        EventQueue& m_Events;
    };
}
