#pragma once
//#include "Windowing/Window.h"


#include <utility>

#include "Core/Events/Event.h"
#include "Core/Events/EventQueue.h"
#include "Core/Events/EventImplementations.h"
#include "Windowing/WindowHandler/WindowHandler.h"
#include "Core/Logging/Log.h"
#include "Renderer/Shader.h"

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

        [[nodiscard]] bool IsMinimized() const
        {
            return m_IsMinimized;
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

        void AddShader(const Ref<Shader>& shader) const;
        void AddShader(std::string_view name, const Ref<Shader>& shader) const;
        [[nodiscard]] Ref<Shader> LoadShader(std::string_view filepath) const;
        [[nodiscard]] Ref<Shader> LoadShader(std::string_view name, std::string_view filepath) const;

        [[nodiscard]] Ref<Shader> GetShader(std::string_view name) const;
        [[nodiscard]] bool ShaderExists(std::string_view name) const;


        void Run();
    protected:
        virtual void Update() {};
        virtual void OnEvent(EventDispatcher& dispatcher) {};

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
    private:
        static OSPlatform s_OSPlatform;
        static Application* s_Instance;

        bool m_IsMinimized;
        WindowHandler* m_Window;
        LayerStack m_Layers;
        EventQueue m_EventQueue;
    };
}