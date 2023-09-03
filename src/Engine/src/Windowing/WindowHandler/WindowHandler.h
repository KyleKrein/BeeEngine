#pragma once

#include "Renderer/GraphicsDevice.h"
#include "Core/TypeDefines.h"
#include "Windowing/WindowProperties.h"
#include "Core/Events/EventQueue.h"
#include "Core/Time.h"
#include "Renderer/Instance.h"
#include "Scripting/ScriptingEngine.h"

namespace BeeEngine
{
    enum class WindowHandlerAPI
    {
        //GLFW = 0,
        SDL = 1
    };
    class WindowHandler
    {
    public:
        virtual ~WindowHandler() = default;
        WindowHandler(const WindowHandler&) = delete;
        WindowHandler& operator=(const WindowHandler&) = delete;
        static gsl::not_null<WindowHandler*> Create(WindowHandlerAPI api, const WindowProperties& properties, EventQueue& eventQueue);
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
        static gsl::not_null<WindowHandler*> GetInstance()
        {
            return s_Instance;
        }
        static WindowHandlerAPI GetAPI()
        {
            return s_API;
        }
        virtual uint64_t GetWindow() = 0;
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
        [[nodiscard]] virtual bool IsRunning() const = 0;
        virtual void UpdateTime() = 0;
        virtual void Close() = 0;

        virtual GraphicsDevice& GetGraphicsDevice() = 0;
        virtual Instance& GetAPIInstance() = 0;

    protected:
        static WindowHandler* s_Instance;
        static WindowHandlerAPI s_API;
        WindowHandler() = delete;
        WindowHandler(EventQueue& eventQueue): m_Width(0), m_Height(0), m_Events(eventQueue) {};

        void UpdateDeltaTime(double currentTime)
        {
            Time::Update(currentTime);
        }
        void SetDeltaTime(double deltaTime, double totalTime)
        {
            Time::Set(deltaTime, totalTime);
            ScriptingEngine::UpdateTime(deltaTime, totalTime);
        }
        uint16_t m_Width;
        uint16_t m_Height;
        const char *m_Title;
        VSync m_vsync;
        EventQueue& m_Events;
    };
}
