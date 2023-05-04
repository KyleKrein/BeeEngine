//
// Created by Александр Лебедев on 23.01.2023.
//

#include "Application.h"
#include "Core/Logging/Log.h"
namespace BeeEngine{
    OSPlatform Application::s_OSPlatform = OSPlatform::None;
    Application* Application::s_Instance = nullptr;
    void Application::Run()
    {
        while (m_Window->IsRunning())
        {
            m_Window->ProcessEvents();
            m_EventQueue.Dispatch();
            m_Layers.Update();
            Update();
        }
    }

    Application::Application(WindowProperties& properties)
    : m_IsMinimized(false), m_Layers(LayerStack()), m_EventQueue(EventQueue(m_Layers))
    {
        BeeCoreAssert(!s_Instance, "You can't have multiple instances of application");
        s_Instance = this;
#ifdef MACOS
        Application::s_OSPlatform = OSPlatform::Mac;
#elif WINDOWS
        Application::s_OSPlatform = OSPlatform::Windows;
#elif LINUX
        Application::s_OSPlatform = OSPlatform::Linux;
#endif


        m_Window = WindowHandler::Create(WindowHandlerAPI::GLFW, properties, m_EventQueue);
        ImGuiLayer guiLayer;
        m_Layers.SetGuiLayer(guiLayer);
    }

    Application::~Application()
    {
        s_Instance = nullptr;
    }

    void Application::Dispatch(EventDispatcher &dispatcher)
    {
        //dispatcher.Dispatch<WindowCloseEvent>(OnWindowClose);
        //dispatcher.Dispatch<WindowCloseEvent>(reinterpret_cast<bool (*)(WindowCloseEvent&)>(OnWindowClose));
    }

    bool Application::OnWindowClose(WindowCloseEvent& event)
    {
        return false;
    }
}

