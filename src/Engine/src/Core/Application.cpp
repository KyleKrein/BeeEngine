//
// Created by Александр Лебедев on 23.01.2023.
//

#include "Application.h"
#include "Core/Logging/Log.h"
#include "Debug/DebugLayer.h"
#include "Renderer/ShaderLibrary.h"

namespace BeeEngine{
    OSPlatform Application::s_OSPlatform = OSPlatform::None;
    Application* Application::s_Instance = nullptr;
    void Application::Run()
    {
        m_EventQueue.AddEvent(CreateScope<WindowResizeEvent>(m_Window->GetWidth(), m_Window->GetHeight()));
        while (m_Window->IsRunning())
        {
            BEE_PROFILE_SCOPE("Application::Run One Frame");
            m_Window->ProcessEvents();
            m_EventQueue.Dispatch();
            m_Window->UpdateTime();
            m_Layers.Update();
            Update();
            m_Window->SwapBuffers();
        }
    }

    Application::Application(WindowProperties& properties)
    : m_IsMinimized(false), m_Layers(LayerStack()), m_EventQueue(EventQueue(m_Layers))
    {
        BEE_PROFILE_FUNCTION();
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

        switch (Application::GetOsPlatform())
        {
            case OSPlatform::Windows:
                Renderer::SetAPI(RenderAPI::OpenGL);
                break;
            case OSPlatform::Mac:
                Renderer::SetAPI(RenderAPI::OpenGL);
                break;
            case OSPlatform::Linux:
                Renderer::SetAPI(RenderAPI::OpenGL);
                break;
            default:
                BeeCoreAssert(false, "Unknown OS Platform");
        }

        m_Layers.SetGuiLayer(new ImGuiLayer());

#ifdef DEBUG
        m_Layers.PushOverlay(CreateRef<Debug::DebugLayer>());
#endif
    }

    Application::~Application()
    {
        s_Instance = nullptr;
        delete m_Window;
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

    void Application::AddShader(const Ref<Shader> &shader) const
    {
        ShaderLibrary::GetInstance().Add(shader);
    }

    void Application::AddShader(std::string_view name, const Ref<Shader> &shader) const
    {
        ShaderLibrary::GetInstance().Add(name, shader);
    }

    Ref<Shader> Application::LoadShader(std::string_view filepath) const
    {
        ShaderLibrary::GetInstance().Load(filepath);
    }

    Ref<Shader> Application::LoadShader(std::string_view name, std::string_view filepath) const
    {
        ShaderLibrary::GetInstance().Load(name, filepath);
    }

    Ref<Shader> Application::GetShader(std::string_view name) const
    {
        return ShaderLibrary::GetInstance().Get(name);
    }

    bool Application::ShaderExists(std::string_view name) const
    {
        return ShaderLibrary::GetInstance().Exists(name);
    }
}

