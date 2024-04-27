//
// Created by Aleksandr on 13.03.2024.
//

#pragma once
#if defined(WINDOWS)
#include "Platform/Vulkan/VulkanGraphicsDevice.h"
#include "Platform/Vulkan/VulkanInstance.h"
#include "WindowHandler.h"
#include "Core/TypeDefines.h"
#include <vulkan/vulkan.hpp>
#include <windows.h>

namespace BeeEngine::Internal
{
    class WinAPIWindowHandler final: public WindowHandler
    {
        friend LRESULT CALLBACK win32_process_message(HWND hwnd, uint32_t msg, WPARAM w_param, LPARAM l_param);
    public:
        WinAPIWindowHandler(const ApplicationProperties& properties, EventQueue& eventQueue);
        ~WinAPIWindowHandler() override;

        GlobalMouseState GetGlobalMouseState() const override;

        WindowNativeInfo GetNativeInfo() override
        {
            return { m_Window, m_WindowsInstance };
        }

        void SetWidth(uint16_t width) override;

        void SetHeight(uint16_t height) override;

        uint64_t GetWindow() override;

        void SetVSync(VSync mode) override;

        void HideCursor() override;

        void DisableCursor() override;

        void ShowCursor() override;

        void ProcessEvents() override;

        [[nodiscard]] bool IsRunning() const override;

        Time::secondsD UpdateTime() override;

        void Close() override;

        GraphicsDevice& GetGraphicsDevice() override
        {
            return *m_GraphicsDevice;
        }

        Instance& GetAPIInstance() override
        {
            return *m_Instance;
        }

        EventQueue& GetEventQueue()
        {
            return m_Events;
        }

    private:
        void InitializeDragDrop();
        HMONITOR GetCurrentMonitor();
    private:
        HINSTANCE m_WindowsInstance;
        HWND m_Window;
        const wchar_t* const m_WindowClassName = L"BeeEngineWindow";
        struct WindowDeleter
        {
            WinAPIWindowHandler* m_WindowHandler;
            ~WindowDeleter()
            {
                ::DestroyWindow(m_WindowHandler->m_Window);
                ::UnregisterClassW(m_WindowHandler->m_WindowClassName, m_WindowHandler->m_WindowsInstance);
            }
        }m_WindowDeleter{this};
        Scope<VulkanInstance> m_Instance;
        Scope<VulkanGraphicsDevice> m_GraphicsDevice;
        mutable bool m_IsRunning = false;
        mutable bool m_IsClosing = false;
    };
}
#endif