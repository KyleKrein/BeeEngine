#pragma once

#include "Core/Events/EventQueue.h"
#include "Core/Time.h"
#include "Core/TypeDefines.h"
#include "Renderer/GraphicsDevice.h"
#include "Renderer/Instance.h"
#include "Scripting/ScriptingEngine.h"
#include "Windowing/ApplicationProperties.h"

namespace BeeEngine
{
    enum class WindowHandlerAPI
    {
        // GLFW = 0,
        SDL = 1,
        WinAPI = 2,
    };
    struct WindowNativeInfo
    {
        void* window = nullptr;
#if defined(WINDOWS)
        void* instance = nullptr;
#elif defined(LINUX)
        void* display = nullptr;
#endif
    };

    struct GlobalMouseState
    {
        int32_t x;
        int32_t y;
        bool left;
        bool right;
        bool middle;
    };
    class WindowHandler
    {
    public:
        virtual ~WindowHandler() = default;
        WindowHandler(const WindowHandler&) = delete;
        WindowHandler& operator=(const WindowHandler&) = delete;
        static gsl::not_null<WindowHandler*>
        Create(WindowHandlerAPI api, const ApplicationProperties& properties, EventQueue& eventQueue);
        uint16_t GetWidth() const { return m_Width; }
        uint16_t GetHeight() const { return m_Height; }
        int32_t GetXPosition() const { return m_XPosition; }
        int32_t GetYPosition() const { return m_YPosition; }
        uint16_t GetWidthInPixels() const { return m_WidthInPixels; }
        uint16_t GetHeightInPixels() const { return m_HeightInPixels; }
        float GetScaleFactor() const { return m_ScaleFactor; }
        virtual GlobalMouseState GetGlobalMouseState() const = 0;
        virtual WindowNativeInfo GetNativeInfo() = 0;
        virtual void SetWidth(uint16_t width) = 0;
        virtual void SetHeight(uint16_t height) = 0;
        static gsl::not_null<WindowHandler*> GetInstance() { return s_Instance; }
        static WindowHandlerAPI GetAPI() { return s_API; }
        virtual uint64_t GetWindow() = 0;
        VSync GetVSync() const { return m_vsync; }
        virtual void SetVSync(VSync mode) = 0;

        virtual void HideCursor() = 0;
        virtual void DisableCursor() = 0;
        virtual void EnableCursor() = 0;
        virtual void ShowCursor() = 0;
        virtual void ProcessEvents() = 0;
        [[nodiscard]] virtual bool IsRunning() const = 0;
        virtual Time::secondsD UpdateTime() = 0;
        virtual void Close() = 0;

        virtual GraphicsDevice& GetGraphicsDevice() = 0;
        virtual Instance& GetAPIInstance() = 0;

    protected:
        static WindowHandler* s_Instance;
        static WindowHandlerAPI s_API;
        WindowHandler() = delete;
        WindowHandler(EventQueue& eventQueue) : m_Width(0), m_Height(0), m_Events(eventQueue) {};

        void UpdateDeltaTime(Time::secondsD currentTime) { Time::Update(currentTime); }
        void SetDeltaTime(Time::secondsD deltaTime, Time::secondsD totalTime)
        {
            BeeCoreTrace("{}", std::source_location::current().function_name());
            Time::Set(deltaTime, totalTime);
#if defined(BEE_ENABLE_SCRIPTING)
            BeeCoreTrace("Updating time in scripting Engine");
            ScriptingEngine::UpdateTime(deltaTime, totalTime);
#endif
        }
        uint16_t m_Width;
        uint16_t m_Height;
        uint16_t m_WidthInPixels;
        uint16_t m_HeightInPixels;
        int32_t m_XPosition;
        int32_t m_YPosition;
        float m_ScaleFactor;
        String m_Title;
        VSync m_vsync;
        EventQueue& m_Events;
    };
} // namespace BeeEngine
