//
// Created by Александр Лебедев on 23.01.2023.
//

#include "Application.h"
#include "Core/Logging/Log.h"
#include "Debug/DebugLayer.h"
// #include "Platform/Vulkan/VulkanRendererAPI.h"
#include "DeletionQueue.h"
#include "JobSystem/JobScheduler.h"
#include "Move.h"
#include "Renderer/GraphicsDevice.h"
#include "Renderer/Renderer.h"
#include "Renderer/RendererAPI.h"
#include "Renderer/SceneRenderer.h"
#include "Scene/Prefab.h"
#include "Scripting/ScriptingEngine.h"
#include <mutex>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
namespace BeeEngine
{
    Application* Application::s_Instance = nullptr;
    std::thread::id Application::s_MainThreadID = {};
    void Application::Run()
    {
        m_EventQueue.AddEvent(CreateScope<WindowResizeEvent>(
            m_Window->GetWidth(), m_Window->GetHeight(), m_Window->GetWidthInPixels(), m_Window->GetHeightInPixels()));
        std::condition_variable cv;
        std::mutex mutex;
        GraphicsDevice& device = m_Window->GetGraphicsDevice();
        device.RequestSwapChainRebuild();
        while (m_Window->IsRunning())
        {
            BEE_PROFILE_SCOPE("Application::Run One Frame");
            ExecuteMainThreadQueue();
            m_Window->ProcessEvents();
            if (device.SwapChainRequiresRebuild())
            {
                Renderer::RebuildSwapchain();
            }
            std::unique_lock lock(mutex);
            auto frameJob = Jobs::CreateJob<Jobs::Priority::Normal, 1024 * 1024>(
                [this](std::condition_variable& cv)
                {
                    m_EventQueue.Dispatch();
                    auto deltaTime = m_Window->UpdateTime();
                    // if(!self.IsMinimized())
                    //{
                    auto result = Renderer::BeginFrame();
                    if (!result.HasValue())
                    {
                        auto error = BeeMove(result).Error();
                        BeeCoreAssert(error == RendererAPI::Error::SwapchainOutdated, "Unhandled RenderedAPI error");
                        cv.notify_one();
                        return;
                    }
                    auto frameData = BeeMove(result).Value();
                    frameData.SetDeltaTime(deltaTime);
                    Renderer::StartMainCommandBuffer(frameData);
                    m_Layers.Update(frameData);
                    Update(frameData);
                    Renderer::EndMainCommandBuffer(frameData);
                    Renderer::EndFrame(frameData);
                    //}
                    // else
                    //{
                    //    self.m_Layers.Update();
                    //    self.Update();
                    //}

                    DeletionQueue::Frame().Flush();

                    cv.notify_one();
                },
                cv);
            Jobs::Schedule(frameJob);
            cv.wait(lock);
        }
        DeletionQueue::Main().Flush();
        BeeEnsures(DeletionQueue::Main().IsEmpty() && DeletionQueue::Frame().IsEmpty() &&
                   DeletionQueue::RendererFlush().IsEmpty());
    }

    Application::Application(const ApplicationProperties& properties)
        : m_IsMinimized(false), m_Layers(), m_EventQueue(m_Layers)
    {
        BEE_PROFILE_FUNCTION();
        BeeCoreAssert(!s_Instance, "You can't have multiple instances of application");
        s_Instance = this;
        s_MainThreadID = std::this_thread::get_id();
        auto appProperties = properties;
        CheckRendererAPIForCompatibility(appProperties);

        m_Window.reset(WindowHandler::Create(GetPreferredWindowAPI(), properties, m_EventQueue));
        Renderer::SetAPI(appProperties.PreferredRenderAPI);

        m_Layers.SetGuiLayer(new ImGuiLayer());

        Prefab::InitPrefabScene();
        m_AssetManager.LoadStandardAssets();
        SceneRenderer::Init();
    }

    Application::~Application()
    {
        s_Instance = nullptr;
        Prefab::ResetPrefabScene();
        Renderer::Shutdown();
    }

    WindowHandlerAPI Application::GetPreferredWindowAPI()
    {
#if defined(WINDOWS)
        return WindowHandlerAPI::WinAPI;
#else
        return WindowHandlerAPI::SDL;
#endif
    }

    void Application::Dispatch(EventDispatcher& dispatcher)
    {
        OnEvent(dispatcher);
    }

    bool Application::OnWindowClose(WindowCloseEvent& event)
    {
        return false;
    }

    void Application::Close()
    {
        m_Window->Close();
    }

    void Application::CheckRendererAPIForCompatibility(ApplicationProperties& properties) noexcept
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
                // properties.PreferredRenderAPI = RenderAPI::WebGPU;
                debug_break();
                return;
        }
    }

    bool Application::OnWindowResize(WindowResizeEvent* event)
    {
        // m_Window->GetGraphicsDevice().WindowResized(event->GetWidth(), event->GetHeight());
        if (event->GetWidthInPoints() == 0 || event->GetWidthInPoints() == 0)
        {
            m_IsMinimized = true;
            return false;
        }
        m_IsMinimized = false;
        return false;
    }

    void Application::SubmitToMainThread_Impl(const std::function<void()>& function)
    {
        std::unique_lock lock(m_MainThreadQueueMutex);
        m_MainThreadQueue.push_back(function);
    }

    void Application::ExecuteMainThreadQueue() noexcept
    {
        std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);
        for (auto& function : m_MainThreadQueue)
        {
            function();
        }
        m_MainThreadQueue.clear();
    }

    RenderAPI GetPrefferedRenderAPI()
    {
        switch (Application::GetOsPlatform())
        {
            case OSPlatform::Windows:
                return RenderAPI::Vulkan;
            case OSPlatform::Linux:
                return RenderAPI::Vulkan;
            case OSPlatform::Mac:
                return RenderAPI::Vulkan;
            case OSPlatform::iOS:
                return RenderAPI::WebGPU;
            case OSPlatform::Android:
                return RenderAPI::Vulkan;
        }
        return RenderAPI::NotAvailable;
    }
} // namespace BeeEngine

#pragma clang diagnostic pop