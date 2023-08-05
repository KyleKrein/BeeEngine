//
// Created by Александр Лебедев on 23.01.2023.
//

#include "Application.h"
#include "Core/Logging/Log.h"
#include "Debug/DebugLayer.h"
//#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Renderer/Renderer.h"
#include "DeletionQueue.h"
#include "Scripting/ScriptingEngine.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
namespace BeeEngine{
    Application* Application::s_Instance = nullptr;
    void Application::Run()
    {
        m_EventQueue.AddEvent(CreateScope<WindowResizeEvent>(m_Window->GetWidth(), m_Window->GetHeight()));
        while (m_Window->IsRunning())
        {
            BEE_PROFILE_SCOPE("Application::Run One Frame");
            ExecuteMainThreadQueue();
            m_Window->ProcessEvents();
            m_EventQueue.Dispatch();
            m_Window->UpdateTime();
            auto cmd = Renderer::BeginFrame();//Internal::VulkanRendererAPI::GetInstance().BeginFrame();
            Renderer::StartMainRenderPass(cmd);//Internal::VulkanRendererAPI::GetInstance().BeginSwapchainRenderPass(cmd);
            m_Layers.Update();
            Update();
            //m_Window->SwapBuffers();
            Renderer::EndMainRenderPass(cmd);//Internal::VulkanRendererAPI::GetInstance().EndSwapchainRenderPass(cmd);
            Renderer::EndFrame();//Internal::VulkanRendererAPI::GetInstance().EndFrame();
            DeletionQueue::Frame().Flush();
        }
    }

    Application::Application(const WindowProperties& properties)
    : m_IsMinimized(false), m_Layers(LayerStack()), m_EventQueue(EventQueue(m_Layers))
    {
        BEE_PROFILE_FUNCTION();
        BeeCoreAssert(!s_Instance, "You can't have multiple instances of application");
        s_Instance = this;
        auto appProperties = properties;
        CheckRendererAPIForCompatibility(appProperties);

        m_Window.reset(WindowHandler::Create(WindowHandlerAPI::SDL, properties, m_EventQueue));
        Renderer::SetAPI(appProperties.PreferredRenderAPI);

        m_Layers.SetGuiLayer(new ImGuiLayer());

#ifdef DEBUG
        m_Layers.PushOverlay(CreateRef<Debug::DebugLayer>());
#endif
        m_AssetManager.LoadStandardAssets();

        //ScriptingEngine::Init();
    }

    Application::~Application()
    {
        s_Instance = nullptr;
    }

    void Application::Dispatch(EventDispatcher &dispatcher)
    {
        DISPATCH_EVENT(dispatcher, WindowResizeEvent, EventType::WindowResize, OnWindowResize);
        //dispatcher.Dispatch<WindowCloseEvent>(OnWindowClose);
        //dispatcher.Dispatch<WindowCloseEvent>(reinterpret_cast<bool (*)(WindowCloseEvent&)>(OnWindowClose));
    }

    bool Application::OnWindowClose(WindowCloseEvent& event)
    {
        return false;
    }

    void Application::Close()
    {
        m_Window->Close();
    }

    void Application::CheckRendererAPIForCompatibility(WindowProperties &properties) noexcept
    {
        switch (properties.PreferredRenderAPI)
        {
            case RenderAPI::WebGPU:
                return;
            case RenderAPI::Vulkan:
                return;
            case RenderAPI::DirectX:
            case RenderAPI::OpenGL:
            case RenderAPI::Metal:
            default:
                BeeCoreWarn("Unable to use {} as render API", ToString(properties.PreferredRenderAPI));
                //properties.PreferredRenderAPI = RenderAPI::OpenGL;
                return;
        }

    }

    bool Application::OnWindowResize(WindowResizeEvent *event)
    {
        //m_Window->GetGraphicsDevice().WindowResized(event->GetWidth(), event->GetHeight());
        if(event->GetWidth() == 0 || event->GetHeight() == 0)
        {
            m_IsMinimized = true;
            return false;
        }
        m_IsMinimized = false;
        return false;
    }

    void Application::SubmitToMainThread_Impl(const std::function<void()> &function)
    {
        std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);
        m_MainThreadQueue.push_back(function);
    }

    void Application::ExecuteMainThreadQueue() noexcept
    {
        std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);
        for(auto& function : m_MainThreadQueue)
        {
            function();
        }
        m_MainThreadQueue.clear();
    }
}


#pragma clang diagnostic pop