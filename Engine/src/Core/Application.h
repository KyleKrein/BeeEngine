#pragma once
#include "Windowing/Window.h"


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
    public:
        Application();
        virtual ~Application();
        static OSPlatform GetOsPlatform()
        {
            return m_OSPlatform;
        }
    private:
        Window* m_Window;
        static OSPlatform m_OSPlatform;
    };
}