#pragma once
//#include "Windowing/Window.h"


#include "Core/Events/Event.h"
#include "Core/Events/EventQueue.h"
#include "Core/Events/EventImplementations.h"

namespace BeeEngine{
    enum class OSPlatform
    {
        None = 0,
        Windows = 1,
        Linux = 2,
        Mac = 3,
        iOS = 4,
        Android = 5
    };
    class Application
    {
        friend EventQueue;
    public:
        Application();
        virtual ~Application();
        static OSPlatform GetOsPlatform()
        {
            return s_OSPlatform;
        }
        static const Application* const GetInstance()
        {
            return s_Instance;
        }

        bool IsMinimized() const
        {
            return m_IsMinimized;
        }

        void Run();
    private:
        void Dispatch(EventDispatcher &dispatcher);
        bool OnWindowClose(WindowCloseEvent& event);
    private:
        //Window* m_Window;
        static OSPlatform s_OSPlatform;
        static Application* s_Instance;
        bool m_IsMinimized;
    };
}