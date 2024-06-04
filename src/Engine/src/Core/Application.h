#pragma once
// #include "Windowing/Window.h"

#include <utility>

#include "Core/Events/Event.h"
#include "Core/Events/EventImplementations.h"
#include "Core/Events/EventQueue.h"
#include "Core/LayerStack.h"
#include "Core/Logging/Log.h"
#include "OsPlatform.h"
#include "Renderer/AssetManager.h"
#include "Renderer/ShaderModule.h"
#include "Windowing/WindowHandler/WindowHandler.h"

namespace BeeEngine
{
    class Application
    {
        friend EventQueue;

    public:
        explicit Application(const ApplicationProperties& properties);
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
        inline static bool IsLittleEndian()
        {
            static int32_t num = 1;
            static bool isLittleEndian = (*(char*)&num == 1);
            return isLittleEndian;
        }
#endif

        static Application& GetInstance() { return *s_Instance; }

        static bool IsMainThread() { return std::this_thread::get_id() == s_MainThreadID; }

        static void SubmitToMainThread(const std::function<void()>& func) { s_Instance->SubmitToMainThread_Impl(func); }

        [[nodiscard]] bool IsMinimized() const { return m_IsMinimized; }
        void AddEvent(Scope<Event> event) { m_EventQueue.AddEvent(std::move(event)); }
        [[nodiscard]] uint16_t GetWidth() const { return m_Window->GetWidth(); }
        void SetWidth(uint16_t width) { m_Window->SetWidth(width); }
        [[nodiscard]] uint16_t GetHeight() const { return m_Window->GetHeight(); }
        void SetHeight(uint16_t height) { m_Window->SetHeight(height); }

        void Run();

        void Close();
        InternalAssetManager& GetAssetManager() { return m_AssetManager; }

        bool IsFocused() { return m_IsFocused; }
        bool IsMaximized() { return m_IsMaximized; }

    protected:
        virtual void Update(FrameData& frameData) {};

        inline void PushLayer(Ref<Layer> layer) { m_Layers.PushLayer(std::move(layer)); }
        inline void PushOverlay(Ref<Layer> overlay) { m_Layers.PushOverlay(std::move(overlay)); }
        inline void PushLayer(Layer&& layer) { m_Layers.PushLayer(std::move(layer)); }
        inline void PushOverlay(Layer&& overlay) { m_Layers.PushOverlay(std::move(overlay)); }
        void PopLayer(Ref<Layer> layer) { m_Layers.PopLayer(std::move(layer)); }
        void PopOverlay(Ref<Layer> overlay) { m_Layers.PopOverlay(std::move(overlay)); }

    private:
        void OnEvent(EventDispatcher& dispatcher)
        {
            dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& event) -> bool
                                                   { return OnWindowResize(&event); });
            dispatcher.Dispatch<WindowMinimizedEvent>(
                [this](WindowMinimizedEvent& event) -> bool
                {
                    m_IsMinimized = event.IsMinimized();
                    return false;
                });
            dispatcher.Dispatch<WindowMaximizedEvent>(
                [this](WindowMaximizedEvent& event) -> bool
                {
                    m_IsMaximized = event.IsMaximized();
                    return false;
                });
            dispatcher.Dispatch<WindowFocusedEvent>(
                [this](auto& event)
                {
                    m_IsFocused = event.IsFocused();
                    return false;
                });
        };
        WindowHandlerAPI GetPreferredWindowAPI();
        void Dispatch(EventDispatcher& dispatcher);
        static bool OnWindowClose(WindowCloseEvent& event);
        void CheckRendererAPIForCompatibility(ApplicationProperties& properties) noexcept;
        bool OnWindowResize(WindowResizeEvent* event);
        void SubmitToMainThread_Impl(const std::function<void()>& function);
        void ExecuteMainThreadQueue() noexcept;

    private:
        std::vector<std::function<void()>> m_MainThreadQueue;
        std::mutex m_MainThreadQueueMutex;
        static Application* s_Instance;
        bool m_IsMinimized = false;
        bool m_IsMaximized = false;
        bool m_IsFocused = true;
        Scope<WindowHandler> m_Window;
        LayerStack m_Layers;
        EventQueue m_EventQueue;
        static std::thread::id s_MainThreadID;

        InternalAssetManager m_AssetManager;
    };
} // namespace BeeEngine