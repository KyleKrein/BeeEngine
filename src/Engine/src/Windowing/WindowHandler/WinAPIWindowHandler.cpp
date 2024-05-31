//
// Created by Aleksandr on 13.03.2024.
//

#include "WinAPIWindowHandler.h"

#include "Platform/Platform.h"

#if defined(WINDOWS)
#include "Core/Events/EventImplementations.h"

#include "Platform/Vulkan/VulkanGraphicsDevice.h"
#include "Platform/Vulkan/VulkanInstance.h"
#include "Platform/Windows/WindowsDropSource.h"
#include "Renderer/Renderer.h"
#include <backends/imgui_impl_win32.h>

#include "Platform/Windows/WindowsString.h"
#include <windowsx.h>
// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace BeeEngine::Internal
{
    void GetMonitorRealResolution(HMONITOR monitor, int* pixelsWidth, int* pixelsHeight)
    {
        MONITORINFOEX info = {sizeof(MONITORINFOEX)};
        GetMonitorInfo(monitor, &info);
        DEVMODE devmode = {};
        devmode.dmSize = sizeof(DEVMODE);
        EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &devmode);
        *pixelsWidth = devmode.dmPelsWidth;
        *pixelsHeight = devmode.dmPelsHeight;
    }
    float GetMonitorScalingRatio(HMONITOR monitor)
    {
        MONITORINFOEX info = {sizeof(MONITORINFOEX)};
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

        ::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        ImGui_ImplWin32_EnableDpiAwareness();

        m_WindowsInstance = GetModuleHandleW(nullptr); // Get the current instance

        std::wstring title = WStringFromUTF8(properties.Title);

        HICON icon = LoadIcon(m_WindowsInstance, IDI_APPLICATION);
        WNDCLASSW wc = {0};
        wc.style = CS_DBLCLKS; // CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = win32_process_message;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = m_WindowsInstance;
        wc.hIcon = icon;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW); // nullptr to manage cursor manually
        wc.hbrBackground = nullptr;
        wc.lpszClassName = m_WindowClassName;

        if (!RegisterClassW(&wc))
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
        // windowStyle |= WS_SIZEBOX;
        windowStyle |= WS_THICKFRAME;

        // Obtain the size of the window borders
        RECT borderRect = {0, 0, 0, 0};
        AdjustWindowRectEx(&borderRect, windowStyle, FALSE, windowExStyle);
        windowWidth += borderRect.right - borderRect.left;
        windowHeight += borderRect.bottom - borderRect.top;
        windowX -= borderRect.left;
        windowY -= borderRect.top;

        m_Window = CreateWindowExW(windowExStyle,
                                   m_WindowClassName,
                                   title.c_str(),
                                   windowStyle,
                                   windowX,
                                   windowY,
                                   windowWidth,
                                   windowHeight,
                                   nullptr,
                                   nullptr,
                                   m_WindowsInstance,
                                   nullptr);

        if (!m_Window)
        {
            BeeCoreError("Window creation failed");
        }

        // Show the window
        bool32_t shouldActivate = true; // If the window should not accept input, this should be false
        int32_t showWindowCommandFlags = shouldActivate ? SW_SHOW : SW_SHOWNOACTIVATE;
        // if initially minimized, use SW_MINIMIZE : SW_SHOWMINNOACTIVATE
        // if initially maximized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE

        m_ScaleFactor = GetRealDpiForMonitor(GetCurrentMonitor());
        m_WidthInPixels = m_ScaleFactor * m_Width;
        m_HeightInPixels = m_ScaleFactor * m_Height;

        InitializeDragDrop();

        BeeExpects(properties.PreferredRenderAPI == RenderAPI::Vulkan);
        m_Instance = CreateScope<VulkanInstance>(std::string_view{properties.Title}, WindowHandlerAPI::WinAPI);
        m_GraphicsDevice = CreateScope<VulkanGraphicsDevice>(*m_Instance);
        ShowWindow(m_Window, showWindowCommandFlags);
        UpdateWindow(m_Window);
        m_IsRunning = true;
    }

    WinAPIWindowHandler::~WinAPIWindowHandler() {}

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
        if (mode == m_vsync)
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
        if (m_IsClosing) [[unlikely]]
        {
            m_IsRunning = false;
            m_IsClosing = false;
            return true;
        }
        return m_IsRunning;
    }

    Time::secondsD WinAPIWindowHandler::UpdateTime()
    {
        static Time::secondsD lastTime;
        static auto currentTime = Time::secondsD{PlatformGetAbsoluteTime()};
        lastTime = currentTime;
        currentTime = Time::secondsD{PlatformGetAbsoluteTime()};
        auto deltatime = currentTime - lastTime;
        SetDeltaTime(deltatime, currentTime);
        return deltatime;
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
    // https://stackoverflow.com/questions/15966642/how-do-you-tell-lshift-apart-from-rshift-in-wm-keydown-events
    WPARAM MapLeftRightKeys(WPARAM vk, LPARAM lParam)
    {
        WPARAM new_vk = vk;
        UINT scancode = (lParam & 0x00ff0000) >> 16;
        int extended = (lParam & 0x01000000) != 0;

        switch (vk)
        {
            case VK_SHIFT:
                new_vk = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
                break;
            case VK_CONTROL:
                new_vk = extended ? VK_RCONTROL : VK_LCONTROL;
                break;
            case VK_MENU:
                new_vk = extended ? VK_RMENU : VK_LMENU;
                break;
            default:
                // not a key we map from generic to left/right specialized
                //  just return it.
                new_vk = vk;
                break;
        }

        return new_vk;
    }

    BeeEngine::Key ConvertKeyCode(WPARAM wParam, LPARAM lParam)
    {
        wParam = MapLeftRightKeys(wParam, lParam);
        switch (wParam)
        {
            case VK_SPACE:
                return BeeEngine::Key::Space;
            case VK_OEM_7:
                return BeeEngine::Key::Apostrophe;
            case VK_OEM_COMMA:
                return BeeEngine::Key::Comma;
            case VK_OEM_MINUS:
                return BeeEngine::Key::Minus;
            case VK_OEM_PERIOD:
                return BeeEngine::Key::Period;
            case VK_OEM_2:
                return BeeEngine::Key::Slash;
            case '0':
                return BeeEngine::Key::D0;
            case '1':
                return BeeEngine::Key::D1;
            case '2':
                return BeeEngine::Key::D2;
            case '3':
                return BeeEngine::Key::D3;
            case '4':
                return BeeEngine::Key::D4;
            case '5':
                return BeeEngine::Key::D5;
            case '6':
                return BeeEngine::Key::D6;
            case '7':
                return BeeEngine::Key::D7;
            case '8':
                return BeeEngine::Key::D8;
            case '9':
                return BeeEngine::Key::D9;
            case 'A':
                return BeeEngine::Key::A;
            case 'B':
                return BeeEngine::Key::B;
            case 'C':
                return BeeEngine::Key::C;
            case 'D':
                return BeeEngine::Key::D;
            case 'E':
                return BeeEngine::Key::E;
            case 'F':
                return BeeEngine::Key::F;
            case 'G':
                return BeeEngine::Key::G;
            case 'H':
                return BeeEngine::Key::H;
            case 'I':
                return BeeEngine::Key::I;
            case 'J':
                return BeeEngine::Key::J;
            case 'K':
                return BeeEngine::Key::K;
            case 'L':
                return BeeEngine::Key::L;
            case 'M':
                return BeeEngine::Key::M;
            case 'N':
                return BeeEngine::Key::N;
            case 'O':
                return BeeEngine::Key::O;
            case 'P':
                return BeeEngine::Key::P;
            case 'Q':
                return BeeEngine::Key::Q;
            case 'R':
                return BeeEngine::Key::R;
            case 'S':
                return BeeEngine::Key::S;
            case 'T':
                return BeeEngine::Key::T;
            case 'U':
                return BeeEngine::Key::U;
            case 'V':
                return BeeEngine::Key::V;
            case 'W':
                return BeeEngine::Key::W;
            case 'X':
                return BeeEngine::Key::X;
            case 'Y':
                return BeeEngine::Key::Y;
            case 'Z':
                return BeeEngine::Key::Z;
            case VK_OEM_1:
                return BeeEngine::Key::Semicolon;
            case VK_OEM_PLUS:
                return BeeEngine::Key::Equal;
            case VK_OEM_4:
                return BeeEngine::Key::LeftBracket;
            case VK_OEM_5:
                return BeeEngine::Key::Backslash;
            case VK_OEM_6:
                return BeeEngine::Key::RightBracket;
            case VK_OEM_3:
                return BeeEngine::Key::GraveAccent;
            case VK_ESCAPE:
                return BeeEngine::Key::Escape;
            case VK_RETURN:
                return BeeEngine::Key::Enter;
            case VK_TAB:
                return BeeEngine::Key::Tab;
            case VK_BACK:
                return BeeEngine::Key::Backspace;
            case VK_INSERT:
                return BeeEngine::Key::Insert;
            case VK_DELETE:
                return BeeEngine::Key::Delete;
            case VK_RIGHT:
                return BeeEngine::Key::Right;
            case VK_LEFT:
                return BeeEngine::Key::Left;
            case VK_DOWN:
                return BeeEngine::Key::Down;
            case VK_UP:
                return BeeEngine::Key::Up;
            case VK_PRIOR:
                return BeeEngine::Key::PageUp;
            case VK_NEXT:
                return BeeEngine::Key::PageDown;
            case VK_HOME:
                return BeeEngine::Key::Home;
            case VK_END:
                return BeeEngine::Key::End;
            case VK_CAPITAL:
                return BeeEngine::Key::CapsLock;
            case VK_SCROLL:
                return BeeEngine::Key::ScrollLock;
            case VK_NUMLOCK:
                return BeeEngine::Key::NumLock;
            case VK_SNAPSHOT:
                return BeeEngine::Key::PrintScreen;
            case VK_PAUSE:
                return BeeEngine::Key::Pause;
            case VK_F1:
                return BeeEngine::Key::F1;
            case VK_F2:
                return BeeEngine::Key::F2;
            case VK_F3:
                return BeeEngine::Key::F3;
            case VK_F4:
                return BeeEngine::Key::F4;
            case VK_F5:
                return BeeEngine::Key::F5;
            case VK_F6:
                return BeeEngine::Key::F6;
            case VK_F7:
                return BeeEngine::Key::F7;
            case VK_F8:
                return BeeEngine::Key::F8;
            case VK_F9:
                return BeeEngine::Key::F9;
            case VK_F10:
                return BeeEngine::Key::F10;
            case VK_F11:
                return BeeEngine::Key::F11;
            case VK_F12:
                return BeeEngine::Key::F12;
            case VK_F13:
                return BeeEngine::Key::F13;
            case VK_F14:
                return BeeEngine::Key::F14;
            case VK_F15:
                return BeeEngine::Key::F15;
            case VK_F16:
                return BeeEngine::Key::F16;
            case VK_F17:
                return BeeEngine::Key::F17;
            case VK_F18:
                return BeeEngine::Key::F18;
            case VK_F19:
                return BeeEngine::Key::F19;
            case VK_F20:
                return BeeEngine::Key::F20;
            case VK_F21:
                return BeeEngine::Key::F21;
            case VK_F22:
                return BeeEngine::Key::F22;
            case VK_F23:
                return BeeEngine::Key::F23;
            case VK_F24:
                return BeeEngine::Key::F24;
            case VK_NUMPAD0:
                return BeeEngine::Key::KeyPad0;
            case VK_NUMPAD1:
                return BeeEngine::Key::KeyPad1;
            case VK_NUMPAD2:
                return BeeEngine::Key::KeyPad2;
            case VK_NUMPAD3:
                return BeeEngine::Key::KeyPad3;
            case VK_NUMPAD4:
                return BeeEngine::Key::KeyPad4;
            case VK_NUMPAD5:
                return BeeEngine::Key::KeyPad5;
            case VK_NUMPAD6:
                return BeeEngine::Key::KeyPad6;
            case VK_NUMPAD7:
                return BeeEngine::Key::KeyPad7;
            case VK_NUMPAD8:
                return BeeEngine::Key::KeyPad8;
            case VK_NUMPAD9:
                return BeeEngine::Key::KeyPad9;
            case VK_DECIMAL:
                return BeeEngine::Key::KeyPadDecimal;
            case VK_DIVIDE:
                return BeeEngine::Key::KeyPadDivide;
            case VK_MULTIPLY:
                return BeeEngine::Key::KeyPadMultiply;
            case VK_SUBTRACT:
                return BeeEngine::Key::KeyPadSubtract;
            case VK_ADD:
                return BeeEngine::Key::KeyPadAdd;
            case VK_LSHIFT:
                return BeeEngine::Key::LeftShift;
            case VK_RSHIFT:
                return BeeEngine::Key::RightShift;
            case VK_LCONTROL:
                return BeeEngine::Key::LeftControl;
            case VK_RCONTROL:
                return BeeEngine::Key::RightControl;
            case VK_LMENU:
                return BeeEngine::Key::LeftAlt;
            case VK_RMENU:
                return BeeEngine::Key::RightAlt;
            case VK_LWIN:
                return BeeEngine::Key::LeftSuper;
            case VK_RWIN:
                return BeeEngine::Key::RightSuper;
            case VK_APPS:
                return BeeEngine::Key::Menu;
                /*case VK_VOLUME_MUTE: return BeeEngine::Key::VolumeMute;
                case VK_VOLUME_DOWN: return BeeEngine::Key::VolumeDown;
                case VK_VOLUME_UP: return BeeEngine::Key::VolumeUp;
                case VK_MEDIA_NEXT_TRACK: return BeeEngine::Key::MediaNextTrack;
                case VK_MEDIA_PREV_TRACK: return BeeEngine::Key::MediaPrevTrack;
                case VK_MEDIA_STOP: return BeeEngine::Key::MediaStop;
                case VK_MEDIA_PLAY_PAUSE: return BeeEngine::Key::MediaPlayPause;
                case VK_BROWSER_BACK: return BeeEngine::Key::BrowserBack;
                case VK_BROWSER_FORWARD: return BeeEngine::Key::BrowserForward;
                case VK_BROWSER_REFRESH: return BeeEngine::Key::BrowserRefresh;
                case VK_BROWSER_STOP: return BeeEngine::Key::BrowserStop;*/
        }
        return BeeEngine::Key::Unknown;
    }

    BeeEngine::MouseButton ConvertMouseButton(UINT message, WPARAM wParam)
    {
        switch (message)
        {
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
                return BeeEngine::MouseButton::Left;
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
                return BeeEngine::MouseButton::Right;
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
                return BeeEngine::MouseButton::Middle;
            case WM_XBUTTONDOWN:
            case WM_XBUTTONUP:
                // Используйте GET_XBUTTON_WPARAM для определения конкретной кнопки
                if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
                {
                    return BeeEngine::MouseButton::Button4;
                }
                else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
                {
                    return BeeEngine::MouseButton::Button5;
                }
                // Можно добавить больше условий, если ваша мышь поддерживает больше дополнительных кнопок
                break;
        }
        return static_cast<BeeEngine::MouseButton>(-1); // Нет соответствия или неизвестная кнопка
    }
#define GET_KEY_REPEAT_COUNT(lParam) (lParam & 0xFFFF)
#define IS_KEY_REPEAT(lParam) ((lParam & 0x40000000) ? true : false)
    LRESULT CALLBACK win32_process_message(HWND hwnd, uint32_t msg, WPARAM w_param, LPARAM l_param)
    {
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, w_param, l_param);

        switch (msg)
        {
            case WM_ERASEBKGND:
                // Notify the OS that erasing will be handled by the application
                return 1;
            case WM_CLOSE:
                WindowHandler::GetInstance()->Close();
                return 0;
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
            case WM_SETFOCUS:
                g_EventQueue->AddEvent(CreateScope<WindowFocusedEvent>(true));
                break;
            case WM_KILLFOCUS:
                g_EventQueue->AddEvent(CreateScope<WindowFocusedEvent>(false));
                break;
            case WM_SYSCOMMAND:
            {
                switch (w_param)
                {
                    case SC_MINIMIZE:
                    {
                        g_EventQueue->AddEvent(CreateScope<WindowMinimizedEvent>(true));
                    }
                    break;
                    case SC_RESTORE:
                    {
                        g_EventQueue->AddEvent(CreateScope<WindowMinimizedEvent>(false));
                    }
                    break;
                    default:
                        break;
                }
            }
            break;
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
                    g_EventQueue->AddEvent(
                        CreateScope<WindowResizeEvent>(width, height, widthInPixels, heightInPixels));
                }
                break;
            }
            case WM_DPICHANGED:
            {
                RECT* suggestedRect = reinterpret_cast<RECT*>(l_param);
                SetWindowPos(hwnd,
                             nullptr,
                             suggestedRect->left,
                             suggestedRect->top,
                             suggestedRect->right - suggestedRect->left,
                             suggestedRect->bottom - suggestedRect->top,
                             SWP_NOZORDER | SWP_NOACTIVATE);
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
                auto key = ConvertKeyCode(w_param, l_param);
                bool8_t isKeyDown = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
                if (isKeyDown)
                    g_EventQueue->AddEvent(
                        CreateScope<KeyPressedEvent>(key, IS_KEY_REPEAT(l_param) ? GET_KEY_REPEAT_COUNT(l_param) : 1));
                else
                    g_EventQueue->AddEvent(CreateScope<KeyReleasedEvent>(key));
            }
            break;
            case WM_MOUSEMOVE:
            {
                int32_t x = GET_X_LPARAM(l_param);
                int32_t y = GET_Y_LPARAM(l_param);
                g_EventQueue->AddEvent(CreateScope<MouseMovedEvent>(x, y));
            }
            break;
            case WM_MOUSEWHEEL:
            {
                int32_t delta = GET_WHEEL_DELTA_WPARAM(w_param);
                if (delta != 0)
                    g_EventQueue->AddEvent(CreateScope<MouseScrolledEvent>(0, delta > 0 ? 1 : -1));
            }
            break;
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP:
            case WM_XBUTTONDOWN:
            case WM_XBUTTONUP:
            {
                bool8_t isDown =
                    (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_XBUTTONDOWN);
                auto button = ConvertMouseButton(msg, w_param);
                if (isDown)
                    g_EventQueue->AddEvent(CreateScope<MouseButtonPressedEvent>(button));
                else
                    g_EventQueue->AddEvent(CreateScope<MouseButtonReleasedEvent>(button));
            }
            break;
            case WM_MOVE:
            {
                s_Instance->m_XPosition = GET_X_LPARAM(l_param);
                s_Instance->m_YPosition = GET_Y_LPARAM(l_param);
                g_EventQueue->AddEvent(CreateScope<WindowMovedEvent>(s_Instance->m_XPosition, s_Instance->m_YPosition));
            }
            break;
            case WM_UNICHAR:
            {
                if (w_param == UNICODE_NOCHAR)
                {
                    // WM_UNICHAR is not sent by Windows, but is sent by some third-party input method engine
                    // Returning TRUE here announces support for this message
                    return TRUE;
                }
                g_EventQueue->AddEvent(CreateScope<CharTypedEvent>(static_cast<char32_t>(w_param)));
            }
            break;
        }

        return ::DefWindowProcW(hwnd, msg, w_param, l_param);
    }
} // namespace BeeEngine::Internal
#endif