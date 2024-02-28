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
#include "Renderer/SceneRenderer.h"
#include "JobSystem/JobScheduler.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
namespace BeeEngine{
    Application* Application::s_Instance = nullptr;
    std::thread::id Application::s_MainThreadID = {};
    void Application::Run()
    {
        m_EventQueue.AddEvent(CreateScope<WindowResizeEvent>(m_Window->GetWidth(), m_Window->GetHeight(), m_Window->GetWidthInPixels(), m_Window->GetHeightInPixels()));
        std::condition_variable cv;
        std::mutex mutex;
        struct FrameJobInfo
        {
            std::condition_variable* cv;
            Application* self;
        } frameJobInfo = {&cv, this};
        while (m_Window->IsRunning())
        {
            BEE_PROFILE_SCOPE("Application::Run One Frame");
            ExecuteMainThreadQueue();
            m_Window->ProcessEvents();
            std::unique_lock lock(mutex);
            Job frameJob{[](void* data){
                auto& frameJobInfo = *reinterpret_cast<FrameJobInfo*>(data);
                auto& self = *frameJobInfo.self;

                self.m_EventQueue.Dispatch();
                self.m_Window->UpdateTime();
                auto cmd = Renderer::BeginFrame();
                Renderer::StartMainRenderPass(cmd);
                self.m_Layers.Update();
                self.Update();
                Renderer::EndMainRenderPass(cmd);
                Renderer::EndFrame();
                DeletionQueue::Frame().Flush();

                frameJobInfo.cv->notify_one();
            }, &frameJobInfo, nullptr,Jobs::Priority::High, 1024*1024};
            Job::Schedule(frameJob);
            cv.wait(lock);
        }
        DeletionQueue::Main().Flush();
    }

    Application::Application(const WindowProperties& properties)
    : m_IsMinimized(false), m_Layers(), m_EventQueue(m_Layers)
    {
        BEE_PROFILE_FUNCTION();
        BeeCoreAssert(!s_Instance, "You can't have multiple instances of application");
        s_Instance = this;
        s_MainThreadID = std::this_thread::get_id();
        auto appProperties = properties;
        CheckRendererAPIForCompatibility(appProperties);

        m_Window.reset(WindowHandler::Create(WindowHandlerAPI::SDL, properties, m_EventQueue));
        Renderer::SetAPI(appProperties.PreferredRenderAPI);

        m_Layers.SetGuiLayer(new ImGuiLayer());

        //m_AssetManager.LoadStandardAssets();
        //SceneRenderer::Init();

        //ScriptingEngine::Init();
    }

    Application::~Application()
    {
        s_Instance = nullptr;
    }

    void Application::Dispatch(EventDispatcher &dispatcher)
    {
        dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& event) -> bool
        {
            return OnWindowResize(&event);
        });
        dispatcher.Dispatch<WindowFocusedEvent>([this](auto& event){
            m_IsFocused = event.IsFocused();
            return false;
        });

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
#if defined(BEE_COMPILE_VULKAN)
            case RenderAPI::Vulkan:
                return;
#else
            case RenderAPI::WebGPU:
                return;
#endif
            default:
                BeeCoreWarn("Unable to use {} as render API", ToString(properties.PreferredRenderAPI));
                //properties.PreferredRenderAPI = RenderAPI::WebGPU;
                debug_break();
                return;
        }

    }

    bool Application::OnWindowResize(WindowResizeEvent *event)
    {
        //m_Window->GetGraphicsDevice().WindowResized(event->GetWidth(), event->GetHeight());
        if(event->GetWidthInPoints() == 0 || event->GetWidthInPoints() == 0)
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

    RenderAPI GetPrefferedRenderAPI()
    {
        switch (Application::GetOsPlatform()) {
            case OSPlatform::Windows:
                return RenderAPI::Vulkan;
            case OSPlatform::Linux:
                return RenderAPI::Vulkan;
            case OSPlatform::Mac:
                return RenderAPI::WebGPU;
            case OSPlatform::iOS:
                return RenderAPI::WebGPU;
            case OSPlatform::Android:
                return RenderAPI::Vulkan;
        }
        return RenderAPI::NotAvailable;
    }
}


#pragma clang diagnostic pop