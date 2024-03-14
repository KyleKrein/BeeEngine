//
// Created by Aleksandr on 13.03.2024.
//

#include "WinAPIWindowHandler.h"

#include "Platform/Platform.h"

#if defined(WINDOWS)
#include "Core/Events/EventImplementations.h"


#include <backends/imgui_impl_win32.h>
#include "Platform/Windows/WindowsDropSource.h"
#include "Renderer/Renderer.h"
#include "Platform/Vulkan/VulkanInstance.h"
#include "Platform/Vulkan/VulkanGraphicsDevice.h"

#include "Platform/Windows/WindowsString.h"
#include <windowsx.h>
// Forward declare message handler from imgui_impl_win32.cpp
    extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace BeeEngine::Internal
{
    void GetMonitorRealResolution(HMONITOR monitor, int* pixelsWidth, int* pixelsHeight)
    {
        MONITORINFOEX info = { sizeof(MONITORINFOEX) };
        GetMonitorInfo(monitor, &info);
        DEVMODE devmode = {};
        devmode.dmSize = sizeof(DEVMODE);
        EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devmode);
        *pixelsWidth = devmode.dmPelsWidth;
        *pixelsHeight = devmode.dmPelsHeight;
    }
    float GetMonitorScalingRatio(HMONITOR monitor)
    {
        MONITORINFOEX info = { sizeof(MONITORINFOEX) };
        GetMonitorInfo(monitor, &info);
        DEVMODE devmode = {};
        devmode.dmSize = sizeof(DEVMODE);
        EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devmode);
        return (info.rcMonitor.right - info.rcMonitor.left) / static_cast<float>(devmode.dmPelsWidth);
    }
    float GetRealDpiForMonitor(HMONITOR monitor)
    {
        return ::GetDpiForSystem() / 96.0 / GetMonitorScalingRatio(monitor);
    }
    static EventQueue* g_EventQueue = nullptr;
    static WinAPIWindowHandler* s_Instance = nullptr;
    LRESULT CALLBACK win32_process_message(HWND hwnd, uint32_t msg, WPARAM w_param, LPARAM l_param);
    WinAPIWindowHandler::WinAPIWindowHandler(const ApplicationProperties& properties, EventQueue& eventQueue)
        : WindowHandler(eventQueue)
    {
        s_Instance = this;
        BeeEngine::Internal::s_Instance = this;
        m_vsync = properties.Vsync;
        g_EventQueue = &eventQueue;

        m_WindowsInstance = GetModuleHandleW(nullptr); //Get the current instance

        std::wstring title = WStringFromUTF8(properties.Title);

        HICON icon = LoadIcon(m_WindowsInstance, IDI_APPLICATION);
        WNDCLASSW wc = {0};
        wc.style = CS_DBLCLKS;//CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = win32_process_message;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = m_WindowsInstance;
        wc.hIcon = icon;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);// nullptr to manage cursor manually
        wc.hbrBackground = nullptr;
        wc.lpszClassName = m_WindowClassName;

        if(!RegisterClassW(&wc))
        {
            BeeCoreError("Failed to register window class");
        }

        m_Width = properties.WindowWidth;
        m_Height = properties.WindowHeight;
        m_XPosition = properties.WindowXPosition;
        m_YPosition = properties.WindowYPosition;
        m_Title = properties.Title;

        int32_t windowX = m_XPosition;
        int32_t windowY = m_YPosition;
        int32_t windowWidth = m_Width;
        int32_t windowHeight = m_Height;

        uint32_t windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
        uint32_t windowExStyle = WS_EX_APPWINDOW;

        windowStyle |= WS_MAXIMIZEBOX;
        windowStyle |= WS_MINIMIZEBOX;
        //windowStyle |= WS_SIZEBOX;
        windowStyle |= WS_THICKFRAME;

        //Obtain the size of the window borders
        RECT borderRect = {0, 0, 0, 0};
        AdjustWindowRectEx(&borderRect, windowStyle, FALSE, windowExStyle);
        windowWidth += borderRect.right - borderRect.left;
        windowHeight += borderRect.bottom - borderRect.top;
        windowX += borderRect.left;
        windowY += borderRect.top;

        m_Window = CreateWindowExW(
            windowExStyle, m_WindowClassName, title.c_str(),
            windowStyle, windowX, windowY,
            windowWidth, windowHeight,
            nullptr, nullptr,
            m_WindowsInstance, nullptr);

        if(!m_Window)
        {
            BeeCoreError("Window creation failed");
        }

        //Show the window
        bool32_t shouldActivate = true; //If the window should not accept input, this should be false
        int32_t showWindowCommandFlags = shouldActivate ? SW_SHOW : SW_SHOWNOACTIVATE;
        //if initially minimized, use SW_MINIMIZE : SW_SHOWMINNOACTIVATE
        //if initially maximized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE

        ::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        ImGui_ImplWin32_EnableDpiAwareness();

        m_ScaleFactor = GetRealDpiForMonitor(GetCurrentMonitor());
        m_WidthInPixels = m_ScaleFactor * m_Width;
        m_HeightInPixels = m_ScaleFactor * m_Height;


        InitializeDragDrop();

        BeeExpects(properties.PreferredRenderAPI == RenderAPI::Vulkan);
        m_Instance = CreateScope<VulkanInstance>(properties.Title, WindowHandlerAPI::WinAPI);
        m_GraphicsDevice = CreateScope<VulkanGraphicsDevice>(*m_Instance);
        ShowWindow(m_Window, showWindowCommandFlags);
        UpdateWindow(m_Window);
        m_IsRunning = true;
    }

    WinAPIWindowHandler::~WinAPIWindowHandler()
    {
    }

    GlobalMouseState WinAPIWindowHandler::GetGlobalMouseState() const
    {
        return GlobalMouseState();
    }

    void WinAPIWindowHandler::SetWidth(uint16_t width)
    {
        /*if(width == m_Width)
            return;
        m_Width = width;
        //::SetWindowPos(m_Window, nullptr, m_XPosition, m_YPosition, m_Width, m_Height, SWP_NOMOVE | SWP_NOZORDER);
        */
    }

    void WinAPIWindowHandler::SetHeight(uint16_t height)
    {
        /*if(height == m_Height)
            return;
        m_Height = height;*/
    }

    uint64_t WinAPIWindowHandler::GetWindow()
    {
        return reinterpret_cast<uint64_t>(m_Window);
    }

    void WinAPIWindowHandler::SetVSync(VSync mode)
    {
        if(mode == m_vsync)
            return;
        m_vsync = mode;
        m_GraphicsDevice->RequestSwapChainRebuild();
    }

    void WinAPIWindowHandler::HideCursor()
    {
        ::ShowCursor(FALSE);
    }

    void WinAPIWindowHandler::DisableCursor()
    {
        //::ShowCursor(FALSE);
    }

    void WinAPIWindowHandler::ShowCursor()
    {
        ::ShowCursor(TRUE);
    }

    void WinAPIWindowHandler::ProcessEvents()
    {
        MSG message;
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    bool WinAPIWindowHandler::IsRunning() const
    {
        if(m_IsClosing) [[unlikely]]
        {
            m_IsRunning = false;
            m_IsClosing = false;
            return true;
        }
        return m_IsRunning;
    }

    void WinAPIWindowHandler::UpdateTime()
    {
        static Time::secondsD lastTime;
        static auto currentTime = Time::secondsD{PlatformGetAbsoluteTime()};
        lastTime = currentTime;
        currentTime = Time::secondsD{PlatformGetAbsoluteTime()};
        auto deltatime = currentTime - lastTime;
        SetDeltaTime(deltatime, currentTime);
    }

    void WinAPIWindowHandler::Close()
    {
        auto event = CreateScope<WindowCloseEvent>();
        m_Events.AddEvent(std::move(event));
        m_IsClosing = true;
    }

    void WinAPIWindowHandler::InitializeDragDrop()
    {
        if (FAILED(OleInitialize(NULL)))
        {
            BeeCoreError("Failed to initialize OLE");
        }
        if (FAILED(RegisterDragDrop(m_Window, static_cast<LPDROPTARGET>(new WindowsDropTarget()))))
        {
            BeeCoreError("Failed to register drop source");
        }
    }

    HMONITOR WinAPIWindowHandler::GetCurrentMonitor()
    {
        return ::MonitorFromWindow(m_Window, MONITOR_DEFAULTTONEAREST);
    }

    BeeEngine::Key ConvertKeyCode(uint32_t key)
    {
        switch(key)
        {

        }
        return BeeEngine::Key::Unknown;
    }

    LRESULT CALLBACK win32_process_message(HWND hwnd, uint32_t msg, WPARAM w_param, LPARAM l_param)
    {
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, w_param, l_param);

        switch (msg)
        {
            case WM_ERASEBKGND:
                //Notify the OS that erasing will be handled by the application
                    return 1;
            case WM_CLOSE:
                WindowHandler::GetInstance()->Close();
                return 0;
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
            case WM_SIZE:
            {
                if (w_param != SIZE_MINIMIZED)
                {
                    RECT rect;
                    GetClientRect(hwnd, &rect);
                    uint32_t width = rect.right - rect.left;
                    uint32_t height = rect.bottom - rect.top;
                    float32_t scaleFactor = s_Instance->m_ScaleFactor;
                    uint32_t widthInPixels = scaleFactor * width;
                    uint32_t heightInPixels = scaleFactor * height;
                    s_Instance->m_Width = width;
                    s_Instance->m_Height = height;
                    s_Instance->m_WidthInPixels = widthInPixels;
                    s_Instance->m_HeightInPixels = heightInPixels;
                    s_Instance->m_GraphicsDevice->RequestSwapChainRebuild();
                    g_EventQueue->AddEvent(CreateScope<WindowResizeEvent>(width, height, widthInPixels, heightInPixels));
                }
               break;
            }
            case WM_DPICHANGED:
            {
                RECT* suggestedRect = reinterpret_cast<RECT*>(l_param);
                SetWindowPos(hwnd, nullptr, suggestedRect->left, suggestedRect->top, suggestedRect->right - suggestedRect->left, suggestedRect->bottom - suggestedRect->top, SWP_NOZORDER | SWP_NOACTIVATE);
                s_Instance->m_ScaleFactor = static_cast<float>(HIWORD(w_param)) / 96.0f;
                s_Instance->m_WidthInPixels = s_Instance->m_ScaleFactor * s_Instance->m_Width;
                s_Instance->m_HeightInPixels = s_Instance->m_ScaleFactor * s_Instance->m_Height;
                s_Instance->m_GraphicsDevice->RequestSwapChainRebuild();
                break;
            }
            case WM_SYSKEYUP:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_KEYDOWN:
            {
                uint32_t keyCode = static_cast<uint32_t>(w_param);
                bool8_t isKeyDown = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
                if(isKeyDown)
                    g_EventQueue->AddEvent(CreateScope<KeyPressedEvent>(ConvertKeyCode(keyCode),0));
                else
                    g_EventQueue->AddEvent(CreateScope<KeyReleasedEvent>(ConvertKeyCode(keyCode)));
            }break;
            case WM_MOUSEMOVE:
            {
                int32_t x = GET_X_LPARAM(l_param);
                int32_t y = GET_Y_LPARAM(l_param);
                g_EventQueue->AddEvent(CreateScope<MouseMovedEvent>(x, y));
            }break;
            case WM_MOUSEWHEEL:
            {
                int32_t delta = GET_WHEEL_DELTA_WPARAM(w_param);
                if(delta != 0)
                    g_EventQueue->AddEvent(CreateScope<MouseScrolledEvent>(0, delta > 0 ? 1 : -1));
            }break;
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP:
            {
                BeeEngine::MouseButton button = BeeEngine::MouseButton::Last;
                if(msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
                    button = BeeEngine::MouseButton::Left;
                else if(msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
                    button = BeeEngine::MouseButton::Right;
                else if(msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
                    button = BeeEngine::MouseButton::Middle;
                bool8_t isDown = (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN);
                if(isDown)
                    g_EventQueue->AddEvent(CreateScope<MouseButtonPressedEvent>(button));
                else
                    g_EventQueue->AddEvent(CreateScope<MouseButtonReleasedEvent>(button));
            }break;
            case WM_MOVE:
            {
                s_Instance->m_XPosition = GET_X_LPARAM(l_param);
                s_Instance->m_YPosition = GET_Y_LPARAM(l_param);
                g_EventQueue->AddEvent(CreateScope<WindowMovedEvent>(s_Instance->m_XPosition, s_Instance->m_YPosition));
            } break;
        }

        return ::DefWindowProcW(hwnd, msg, w_param, l_param);
    }
}
#endif