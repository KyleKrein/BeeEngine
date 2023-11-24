#pragma once
//#include "Windowing/Window.h"


#include <utility>

#include "Core/Events/Event.h"
#include "Core/Events/EventQueue.h"
#include "Core/Events/EventImplementations.h"
#include "Windowing/WindowHandler/WindowHandler.h"
#include "Core/Logging/Log.h"
#include "Renderer/ShaderModule.h"
#include "OsPlatform.h"
#include "Renderer/AssetManager.h"

namespace BeeEngine{
    class Application
    {
        friend EventQueue;
    public:
        explicit Application(const WindowProperties& properties);
        virtual ~Application();
        consteval static OSPlatform GetOsPlatform()
        {
#if (defined(MACOS))
            return OSPlatform::Mac;
#elif defined(WINDOWS)
            return OSPlatform::Windows;
#elif defined(LINUX)
            return OSPlatform::Linux;
#elif defined(ANDROID)
            return OSPlatform::Android;
#elif defined(IOS)
            return OSPlatform::iOS;
#else
            return OSPlatform::None;
#endif
        }
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        static consteval bool IsLittleEndian() { return true; }
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        static consteval bool IsLittleEndian() { return false; }
#else
        static bool IsLittleEndian()
        {
            int32_t num = 1;
            return (*(char*)&num == 1);
        }
#endif


        static Application& GetInstance()
        {
            return *s_Instance;
        }

        static void SubmitToMainThread(const std::function<void()>& func)
        {
            s_Instance->SubmitToMainThread_Impl(func);
        }

        [[nodiscard]] bool IsMinimized() const
        {
            return m_IsMinimized;
        }
        void AddEvent(Scope<Event> event)
        {
            m_EventQueue.AddEvent(std::move(event));
        }
        [[nodiscard]] uint16_t GetWidth() const
        {
            return m_Window->GetWidth();
        }
        void SetWidth(uint16_t width)
        {
            m_Window->SetWidth(width);
        }
        [[nodiscard]] uint16_t GetHeight() const
        {
            return m_Window->GetHeight();
        }
        void SetHeight(uint16_t height)
        {
            m_Window->SetHeight(height);
        }

        void Run();

        void Close();
        InternalAssetManager& GetAssetManager()
        {
            return m_AssetManager;
        }

        bool IsFocused()
        {
            return m_IsFocused;
        }

    protected:
        virtual void Update() {};
        virtual void OnEvent(EventDispatcher& dispatcher)
        {
            dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& event) -> bool
            {
                return OnWindowResize(&event);
            });
        };

        inline void PushLayer(Ref<Layer> layer)
        {
            m_Layers.PushLayer(std::move(layer));
        }
        inline void PushOverlay(Ref<Layer> overlay)
        {
            m_Layers.PushOverlay(std::move(overlay));
        }
        inline void PushLayer(Layer&& layer)
        {
            m_Layers.PushLayer(std::move(layer));
        }
        inline void PushOverlay(Layer&& overlay)
        {
            m_Layers.PushOverlay(std::move(overlay));
        }
    private:
        void Dispatch(EventDispatcher &dispatcher);
        static bool OnWindowClose(WindowCloseEvent& event);
        void CheckRendererAPIForCompatibility(WindowProperties &properties) noexcept;
        bool OnWindowResize(WindowResizeEvent* event);
        void SubmitToMainThread_Impl(const std::function<void()> &function);
        void ExecuteMainThreadQueue() noexcept;
    private:
        std::vector<std::function<void()>> m_MainThreadQueue;
        std::mutex m_MainThreadQueueMutex;
        static Application* s_Instance;
        bool m_IsMinimized;
        bool m_IsFocused;
        Scope<WindowHandler> m_Window;
        LayerStack m_Layers;
        EventQueue m_EventQueue;

        InternalAssetManager m_AssetManager;
    };
}