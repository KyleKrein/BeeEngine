#pragma once
//#include "Windowing/Window.h"


#include "Core/Events/Event.h"
#include "Core/Events/EventQueue.h"
#include "Core/Events/EventImplementations.h"
#include "Windowing/WindowHandler/WindowHandler.h"
#include "Core/Logging/Log.h"

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
        explicit Application(WindowProperties& properties);
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
        uint16_t GetWidth() const
        {
            return m_Window->GetWidth();
        }
        void SetWidth(uint16_t width)
        {
            m_Window->SetWidth(width);
        }
        uint16_t GetHeight() const
        {
            return m_Window->GetHeight();
        }
        void SetHeight(uint16_t height)
        {
            m_Window->SetHeight(height);
        }


        void Run();
    protected:
        virtual void Update() {};
        virtual void OnEvent(EventDispatcher& dispatcher) {};

        void PushLayer(Layer& layer)
        {
            m_Layers.PushLayer(layer);
        }
        void PushOverlay(Layer& overlay)
        {
            m_Layers.PushOverlay(overlay);
        }
    private:
        void Dispatch(EventDispatcher &dispatcher);
        static bool OnWindowClose(WindowCloseEvent& event);
    private:
        static OSPlatform s_OSPlatform;
        static Application* s_Instance;

        bool m_IsMinimized;
        WindowHandler* m_Window;
        LayerStack m_Layers;
        EventQueue m_EventQueue;
    };
}