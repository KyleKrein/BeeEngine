//
// Created by Александр Лебедев on 28.06.2023.
//

#include "SDLWindowHandler.h"
#include "Core/Application.h"
#include "Platform/Vulkan/VulkanInstance.h"
#include "Platform/Vulkan/VulkanGraphicsDevice.h"
#include "backends/imgui_impl_sdl3.h"
#include "Platform/WebGPU/WebGPUInstance.h"
#include "Platform/WebGPU/WebGPUGraphicsDevice.h"
#include "Hardware.h"

#if defined(WINDOWS)
#include "Platform/Windows/WindowsDropSource.h"
#endif

#if defined(MACOS)
#include "Platform/MacOS/MacOSDragDrop.h"
#endif

namespace BeeEngine::Internal
{

    SDLWindowHandler::SDLWindowHandler(const WindowProperties &properties, EventQueue &eventQueue)
    : WindowHandler(eventQueue), m_Finalizer()
    {
        s_Instance = this;
        m_vsync = properties.Vsync;
        auto result = SDL_Init(SDL_INIT_VIDEO);
        if(result != 0)
        {
            BeeCoreError("Failed to initialize SDL3! {}", SDL_GetError());
        }

        int windowFlags = SDL_WINDOW_RESIZABLE;

        switch (properties.PreferredRenderAPI)
        {
            //case Vulkan:
            //    windowFlags |= SDL_WINDOW_VULKAN;
            //    break;
            case WebGPU:
                if constexpr (Application::GetOsPlatform() == OSPlatform::Mac || Application::GetOsPlatform() == OSPlatform::iOS)
                {
                    windowFlags |= SDL_WINDOW_METAL;
                }
                break;
            default:
            BeeCoreFatalError("Invalid Renderer API chosen for SDL");
        }

        if(Application::GetOsPlatform() == OSPlatform::Mac || Application::GetOsPlatform() == OSPlatform::iOS)
        {
            windowFlags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;
        }
        m_Width = properties.Width;
        m_Height = properties.Height;
        m_Title = properties.Title;

        m_Window = SDL_CreateWindow(properties.Title, gsl::narrow_cast<int>(m_Width), gsl::narrow_cast<int>(m_Height), windowFlags);
        if(m_Window == nullptr)
        {
            BeeCoreError("Failed to create SDL3 window! {}", SDL_GetError());
        }
        int widthInPixels, heightInPixels;
        SDL_GetWindowSizeInPixels(m_Window, &widthInPixels, &heightInPixels);
        m_WidthInPixels = widthInPixels;
        m_HeightInPixels = heightInPixels;
        m_ScaleFactor = (float32_t)m_WidthInPixels / (float32_t)m_Width;
        int posX, posY;
        SDL_GetWindowPosition(m_Window, &posX, &posY);
        m_XPosition = posX;
        m_YPosition = posY;
        InitializeDragDropOnWindows();
#if defined(MACOS)
        IntegrateDragAndDropSDL();
#endif
        switch (properties.PreferredRenderAPI)
        {
#if defined(BEE_COMPILE_VULKAN)
            case Vulkan:
                InitializeVulkan();
                break;
#endif
            case WebGPU:
                InitializeWebGPU();
                break;
            default:
                BeeCoreFatalError("Invalid Renderer API chosen for SDL");
        }
        m_IsRunning = true;
    }

    void SDLWindowHandler::InitializeVulkan()
    {
#if defined(BEE_COMPILE_VULKAN)
        m_Instance = CreateScope<VulkanInstance>(m_Title, WindowHandlerAPI::SDL);
        m_GraphicsDevice = CreateScope<VulkanGraphicsDevice>(*(VulkanInstance*)m_Instance.get());
#endif
    }


    void SDLWindowHandler::InitializeWebGPU()
    {
#if defined(BEE_COMPILE_WEBGPU)
        m_Instance = CreateScope<WebGPUInstance>();
        m_GraphicsDevice = CreateScope<WebGPUGraphicsDevice>(*(WebGPUInstance*)m_Instance.get());
#endif
    }

    SDLWindowHandler::~SDLWindowHandler()
    {
        m_Finalizer.window = m_Window;
        DeletionQueue::Main().Flush();
    }

    void SDLWindowHandler::SetWidth(uint16_t width)
    {
        if(Application::GetOsPlatform() == OSPlatform::Mac || Application::GetOsPlatform() == OSPlatform::iOS)
        {
            m_Width = width;
        }
        else
        {
            m_Width = width;
        }
        SDL_SetWindowSize(m_Window, m_Width, m_Height);
        //m_GraphicsDevice->WindowResized(m_Width, m_Height);
    }

    void SDLWindowHandler::SetHeight(uint16_t height)
    {
        if(Application::GetOsPlatform() == OSPlatform::Mac || Application::GetOsPlatform() == OSPlatform::iOS)
        {
            m_Height = height;
        }
        else
        {
            m_Height = height;
        }
        SDL_SetWindowSize(m_Window, m_Width, m_Height);
    }

    void SDLWindowHandler::SetVSync(VSync mode)
    {
        if(mode == m_vsync)
            return;
        m_vsync = mode;
        m_GraphicsDevice->RequestSwapChainRebuild();
    }

    void SDLWindowHandler::HideCursor()
    {
        SDL_HideCursor();
    }

    void SDLWindowHandler::DisableCursor()
    {

    }

    void SDLWindowHandler::ShowCursor()
    {
        SDL_ShowCursor();
    }

    void SDLWindowHandler::ProcessEvents()
    {
        SDL_Event sdlEvent;
        static Scope<FileDropEvent> fileDropEvent = nullptr;
        while (SDL_PollEvent(&sdlEvent))
        {
            ImGui_ImplSDL3_ProcessEvent(&sdlEvent);
            switch (sdlEvent.type)
            {
                case SDL_EVENT_QUIT:
                {
                    Close();
                    break;
                }
                case SDL_EVENT_WINDOW_RESIZED:
                {
                    m_Width = sdlEvent.window.data1;
                    m_Height = sdlEvent.window.data2;
                    int width, height;
                    SDL_GetWindowSizeInPixels(m_Window, &width, &height);
                    m_WidthInPixels = width;
                    m_HeightInPixels = height;
                    m_ScaleFactor = (float32_t)m_WidthInPixels / (float32_t)m_Width;
                    m_GraphicsDevice->RequestSwapChainRebuild();
                    auto event = CreateScope<WindowResizeEvent>(m_Width, m_Height, m_WidthInPixels, m_HeightInPixels);
                    m_Events.AddEvent(std::move(event));
                    break;
                }
                case SDL_EVENT_WINDOW_MINIMIZED:
                {
                    break;
                }
                case SDL_EVENT_WINDOW_MAXIMIZED:
                {
                    break;
                }
                case SDL_EVENT_WINDOW_RESTORED:
                {
                    break;
                }
                case SDL_EVENT_WINDOW_MOUSE_ENTER:
                {
                    //BeeCoreTrace("Mouse entered window");
                    break;
                }
                case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                {
                    //BeeCoreTrace("Mouse left window");
                    break;
                }
                case SDL_EVENT_WINDOW_FOCUS_GAINED:
                {
                    auto event = CreateScope<WindowFocusedEvent>(true);
                    m_Events.AddEvent(std::move(event));
                    break;
                }
                case SDL_EVENT_WINDOW_FOCUS_LOST:
                {
                    auto event = CreateScope<WindowFocusedEvent>(false);
                    m_Events.AddEvent(std::move(event));
                    break;
                }
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                {
                    Close();
                    break;
                }
                case SDL_EVENT_WINDOW_TAKE_FOCUS:
                {
                    break;
                }
                case SDL_EVENT_WINDOW_MOVED:
                {
                    m_XPosition = sdlEvent.window.data1;
                    m_YPosition = sdlEvent.window.data2;
                    auto event = CreateScope<WindowMovedEvent>(m_XPosition, m_YPosition);
                    m_Events.AddEvent(std::move(event));
                    break;
                }
                case SDL_EVENT_WINDOW_HIT_TEST:
                {
                    break;
                }
                case SDL_EVENT_KEY_UP:
                {
                    auto event = CreateScope<KeyReleasedEvent>(ConvertKeyCode(sdlEvent.key.keysym.scancode));
                    m_Events.AddEvent(std::move(event));
                    break;
                }
                case SDL_EVENT_KEY_DOWN:
                {
                    auto event = CreateScope<KeyPressedEvent>(ConvertKeyCode(sdlEvent.key.keysym.scancode), sdlEvent.key.repeat);
                    m_Events.AddEvent(std::move(event));
                    break;
                }
                case SDL_EVENT_TEXT_EDITING:
                {
                    break;
                }
                case SDL_EVENT_TEXT_INPUT:
                {
                    UTF8String string = sdlEvent.text.text;
                    UTF8StringView text(string);
                    for (char32_t c : text)
                    {
                        auto event = CreateScope<CharTypedEvent>(c);
                        m_Events.AddEvent(std::move(event));
                    }
                    break;
                }
                case SDL_EVENT_KEYMAP_CHANGED:
                {
                    break;
                }
                case SDL_EVENT_MOUSE_MOTION:
                {
                    auto event = CreateScope<MouseMovedEvent>(sdlEvent.motion.x, sdlEvent.motion.y);
                    m_Events.AddEvent(std::move(event));
                    break;
                }
                case SDL_EVENT_MOUSE_BUTTON_UP:
                {
                    auto event = CreateScope<MouseButtonReleasedEvent>(ConvertMouseButton(sdlEvent.button.button));
                    m_Events.AddEvent(std::move(event));
                    break;
                }
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                {
                    auto event = CreateScope<MouseButtonPressedEvent>(ConvertMouseButton(sdlEvent.button.button));
                    m_Events.AddEvent(std::move(event));
                    break;
                }
                case SDL_EVENT_MOUSE_WHEEL:
                {
                    auto event = CreateScope<MouseScrolledEvent>(sdlEvent.wheel.x, sdlEvent.wheel.y);
                    m_Events.AddEvent(std::move(event));
                    break;
                }
#if !defined(MACOS)
                case SDL_EVENT_DROP_BEGIN:
                {
                    fileDropEvent = CreateScope<FileDropEvent>();
                    break;
                }
                case SDL_EVENT_DROP_FILE:
                {
                    char* path = sdlEvent.drop.data;
                    fileDropEvent->AddFile(path);
                    break;
                }
                case SDL_EVENT_DROP_COMPLETE:
                {
                    m_Events.AddEvent(std::move(fileDropEvent));
                    break;
                }
                case SDL_EVENT_DROP_TEXT:
                {
                    break;
                }
                case SDL_EVENT_DROP_POSITION:
                {
                    auto event = CreateScope<FileDragEvent>(sdlEvent.drop.x, sdlEvent.drop.y);
                    m_Events.AddEvent(std::move(event));
                    break;
                }
#endif
                case SDL_EVENT_JOYSTICK_AXIS_MOTION:
                {
                    break;
                }
                case SDL_EVENT_JOYSTICK_HAT_MOTION:
                {
                    break;
                }
                case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
                {
                    break;
                }
                case SDL_EVENT_JOYSTICK_BUTTON_UP:
                {
                    break;
                }
                case SDL_EVENT_JOYSTICK_ADDED:
                {
                    break;
                }
                case SDL_EVENT_JOYSTICK_REMOVED:
                {
                    break;
                }
                case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                {
                    break;
                }
                case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                {
                    break;
                }
                case SDL_EVENT_GAMEPAD_BUTTON_UP:
                {
                    break;
                }
                case SDL_EVENT_GAMEPAD_ADDED:
                {
                    break;
                }
                case SDL_EVENT_GAMEPAD_REMOVED:
                {
                    break;
                }
                case SDL_EVENT_GAMEPAD_REMAPPED:
                {
                    break;
                }
                case SDL_EVENT_FINGER_DOWN:
                {
                    break;
                }
                case SDL_EVENT_FINGER_UP:
                {
                    break;
                }
                case SDL_EVENT_FINGER_MOTION:
                {
                    break;
                }
                case SDL_EVENT_CLIPBOARD_UPDATE:
                {
                    break;
                }
                case SDL_EVENT_AUDIO_DEVICE_ADDED:
                {
                    break;
                }
                case SDL_EVENT_AUDIO_DEVICE_REMOVED:
                {
                    break;
                }
                case SDL_EVENT_USER:
                {
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }

    bool SDLWindowHandler::IsRunning() const
    {
        if(m_IsClosing) [[unlikely]]
        {
            m_IsRunning = false;
            m_IsClosing = false;
            return true;
        }
        return m_IsRunning;
    }

    void SDLWindowHandler::UpdateTime()
    {
        static uint64_t lastTime = 0;
        static uint64_t currentTime = SDL_GetPerformanceCounter();
        lastTime = currentTime;
        currentTime = SDL_GetPerformanceCounter();
        auto deltatime = ((double)(currentTime - lastTime)) / (double)SDL_GetPerformanceFrequency();
        SetDeltaTime(Time::secondsD(deltatime), Time::secondsD(((double)currentTime)/(double)SDL_GetPerformanceFrequency()));
        //UpdateDeltaTime(gsl::narrow_cast<float>(SDL_GetPerformanceCounter()));
    }

    void SDLWindowHandler::Close()
    {
        auto event = CreateScope<WindowCloseEvent>();
        m_Events.AddEvent(std::move(event));
        m_IsClosing = true;
    }

    Key SDLWindowHandler::ConvertKeyCode(SDL_Scancode key)
    {
        switch (key)
        {
            case SDL_SCANCODE_UNKNOWN:
                return Key::Unknown;

            case SDL_SCANCODE_A:
                return Key::A;

            case SDL_SCANCODE_B:
                return Key::B;

            case SDL_SCANCODE_C:
                return Key::C;

            case SDL_SCANCODE_D:
                return Key::D;

            case SDL_SCANCODE_E:
                return Key::E;

            case SDL_SCANCODE_F:
                return Key::F;

            case SDL_SCANCODE_G:
                return Key::G;

            case SDL_SCANCODE_H:
                return Key::H;

            case SDL_SCANCODE_I:
                return Key::I;

            case SDL_SCANCODE_J:
                return Key::J;

            case SDL_SCANCODE_K:
                return Key::K;

            case SDL_SCANCODE_L:
                return Key::L;

            case SDL_SCANCODE_M:
                return Key::M;

            case SDL_SCANCODE_N:
                return Key::N;

            case SDL_SCANCODE_O:
                return Key::O;

            case SDL_SCANCODE_P:
                return Key::P;

            case SDL_SCANCODE_Q:
                return Key::Q;

            case SDL_SCANCODE_R:
                return Key::R;

            case SDL_SCANCODE_S:
                return Key::S;

            case SDL_SCANCODE_T:
                return Key::T;

            case SDL_SCANCODE_U:
                return Key::U;

            case SDL_SCANCODE_V:
                return Key::V;

            case SDL_SCANCODE_W:
                return Key::W;

            case SDL_SCANCODE_X:
                return Key::X;

            case SDL_SCANCODE_Y:
                return Key::Y;

            case SDL_SCANCODE_Z:
                return Key::Z;

            case SDL_SCANCODE_1:
                return Key::D1;

            case SDL_SCANCODE_2:
                return Key::D2;

            case SDL_SCANCODE_3:
                return Key::D3;

            case SDL_SCANCODE_4:
                return Key::D4;

            case SDL_SCANCODE_5:
                return Key::D5;

            case SDL_SCANCODE_6:
                return Key::D6;

            case SDL_SCANCODE_7:
                return Key::D7;

            case SDL_SCANCODE_8:
                return Key::D8;

            case SDL_SCANCODE_9:
                return Key::D9;

            case SDL_SCANCODE_0:
                return Key::D0;

            case SDL_SCANCODE_RETURN:
                return Key::Enter;

            case SDL_SCANCODE_ESCAPE:
                return Key::Escape;

            case SDL_SCANCODE_BACKSPACE:
                return Key::Backspace;

            case SDL_SCANCODE_TAB:
                return Key::Tab;

            case SDL_SCANCODE_SPACE:
                return Key::Space;

            case SDL_SCANCODE_MINUS:
                return Key::Minus;

            case SDL_SCANCODE_EQUALS:
                return Key::Equal;

            case SDL_SCANCODE_LEFTBRACKET:
                return Key::LeftBracket;

            case SDL_SCANCODE_RIGHTBRACKET:
                return Key::RightBracket;

            case SDL_SCANCODE_BACKSLASH:
                return Key::Backslash;

            case SDL_SCANCODE_NONUSHASH:
                return Key::Unknown;

            case SDL_SCANCODE_SEMICOLON:
                return Key::Semicolon;

            case SDL_SCANCODE_APOSTROPHE:
                return Key::Apostrophe;

            case SDL_SCANCODE_GRAVE:
                return Key::GraveAccent;

            case SDL_SCANCODE_COMMA:
                return Key::Comma;

            case SDL_SCANCODE_PERIOD:
                return Key::Period;

            case SDL_SCANCODE_SLASH:
                return Key::Slash;

            case SDL_SCANCODE_CAPSLOCK:
                return Key::CapsLock;

            case SDL_SCANCODE_F1:
                return Key::F1;

            case SDL_SCANCODE_F2:
                return Key::F2;

            case SDL_SCANCODE_F3:
                return Key::F3;

            case SDL_SCANCODE_F4:
                return Key::F4;

            case SDL_SCANCODE_F5:
                return Key::F5;

            case SDL_SCANCODE_F6:
                return Key::F6;

            case SDL_SCANCODE_F7:
                return Key::F7;

            case SDL_SCANCODE_F8:
                return Key::F8;

            case SDL_SCANCODE_F9:
                return Key::F9;

            case SDL_SCANCODE_F10:
                return Key::F10;

            case SDL_SCANCODE_F11:
                return Key::F11;

            case SDL_SCANCODE_F12:
                return Key::F12;

            case SDL_SCANCODE_PRINTSCREEN:
                return Key::PrintScreen;

            case SDL_SCANCODE_SCROLLLOCK:
                return Key::ScrollLock;

            case SDL_SCANCODE_PAUSE:
                return Key::Pause;

            case SDL_SCANCODE_INSERT:
                return Key::Insert;

            case SDL_SCANCODE_HOME:
                return Key::Home;

            case SDL_SCANCODE_PAGEUP:
                return Key::PageUp;

            case SDL_SCANCODE_DELETE:
                return Key::Delete;

            case SDL_SCANCODE_END:
                return Key::End;

            case SDL_SCANCODE_PAGEDOWN:
                return Key::PageDown;

            case SDL_SCANCODE_RIGHT:
                return Key::Right;

            case SDL_SCANCODE_LEFT:
                return Key::Left;

            case SDL_SCANCODE_DOWN:
                return Key::Down;

            case SDL_SCANCODE_UP:
                return Key::Up;
                break;
            case SDL_SCANCODE_NUMLOCKCLEAR:
                return Key::NumLock;
                break;
            case SDL_SCANCODE_KP_DIVIDE:
                return Key::KeyPadDivide;
                break;
            case SDL_SCANCODE_KP_MULTIPLY:
                return Key::KeyPadMultiply;
                break;
            case SDL_SCANCODE_KP_MINUS:
                return Key::KeyPadSubtract;
                break;
            case SDL_SCANCODE_KP_PLUS:
                return Key::KeyPadAdd;
                break;
            case SDL_SCANCODE_KP_ENTER:
                return Key::KeyPadEnter;
                break;
            case SDL_SCANCODE_KP_1:
                return Key::KeyPad1;
                break;
            case SDL_SCANCODE_KP_2:
                return Key::KeyPad2;
                break;
            case SDL_SCANCODE_KP_3:
                return Key::KeyPad3;
                break;
            case SDL_SCANCODE_KP_4:
                return Key::KeyPad4;
                break;
            case SDL_SCANCODE_KP_5:
                return Key::KeyPad5;
                break;
            case SDL_SCANCODE_KP_6:
                return Key::KeyPad6;
                break;
            case SDL_SCANCODE_KP_7:
                return Key::KeyPad7;
                break;
            case SDL_SCANCODE_KP_8:
                return Key::KeyPad8;
                break;
            case SDL_SCANCODE_KP_9:
                return Key::KeyPad9;
                break;
            case SDL_SCANCODE_KP_0:
                return Key::KeyPad0;
                break;
            case SDL_SCANCODE_KP_PERIOD:
                return Key::KeyPadDecimal;
                break;
            case SDL_SCANCODE_NONUSBACKSLASH:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_APPLICATION:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_POWER:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_EQUALS:
                return Key::KeyPadEqual;
                break;
            case SDL_SCANCODE_F13:
                return Key::F13;
                break;
            case SDL_SCANCODE_F14:
                return Key::F14;
                break;
            case SDL_SCANCODE_F15:
                return Key::F15;
                break;
            case SDL_SCANCODE_F16:
                return Key::F16;
                break;
            case SDL_SCANCODE_F17:
                return Key::F17;
                break;
            case SDL_SCANCODE_F18:
                return Key::F18;
                break;
            case SDL_SCANCODE_F19:
                return Key::F19;
                break;
            case SDL_SCANCODE_F20:
                return Key::F20;
                break;
            case SDL_SCANCODE_F21:
                return Key::F21;
                break;
            case SDL_SCANCODE_F22:
                return Key::F22;
                break;
            case SDL_SCANCODE_F23:
                return Key::F23;
                break;
            case SDL_SCANCODE_F24:
                return Key::F24;
                break;
            case SDL_SCANCODE_EXECUTE:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_HELP:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_MENU:
                return Key::Menu;
                break;
            case SDL_SCANCODE_SELECT:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_STOP:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_AGAIN:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_UNDO:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_CUT:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_COPY:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_PASTE:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_FIND:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_MUTE:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_VOLUMEUP:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_VOLUMEDOWN:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_COMMA:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_EQUALSAS400:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_INTERNATIONAL1:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_INTERNATIONAL2:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_INTERNATIONAL3:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_INTERNATIONAL4:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_INTERNATIONAL5:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_INTERNATIONAL6:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_INTERNATIONAL7:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_INTERNATIONAL8:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_INTERNATIONAL9:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_LANG1:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_LANG2:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_LANG3:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_LANG4:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_LANG5:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_LANG6:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_LANG7:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_LANG8:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_LANG9:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_ALTERASE:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_SYSREQ:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_CANCEL:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_CLEAR:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_PRIOR:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_RETURN2:
                return Key::Enter;
                break;
            case SDL_SCANCODE_SEPARATOR:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_OUT:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_OPER:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_CLEARAGAIN:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_CRSEL:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_EXSEL:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_00:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_000:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_THOUSANDSSEPARATOR:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_DECIMALSEPARATOR:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_CURRENCYUNIT:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_CURRENCYSUBUNIT:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_LEFTPAREN:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_RIGHTPAREN:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_LEFTBRACE:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_RIGHTBRACE:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_TAB:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_BACKSPACE:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_A:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_B:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_C:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_D:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_E:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_F:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_XOR:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_POWER:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_PERCENT:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_LESS:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_GREATER:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_AMPERSAND:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_DBLAMPERSAND:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_VERTICALBAR:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_DBLVERTICALBAR:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_COLON:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_HASH:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_SPACE:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_AT:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_EXCLAM:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_MEMSTORE:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_MEMRECALL:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_MEMCLEAR:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_MEMADD:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_MEMSUBTRACT:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_MEMMULTIPLY:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_MEMDIVIDE:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_PLUSMINUS:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_CLEAR:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_CLEARENTRY:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_BINARY:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_OCTAL:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_DECIMAL:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KP_HEXADECIMAL:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_LCTRL:
                return Key::LeftControl;
                break;
            case SDL_SCANCODE_LSHIFT:
                return Key::LeftShift;
                break;
            case SDL_SCANCODE_LALT:
                return Key::LeftAlt;
                break;
            case SDL_SCANCODE_LGUI:
                return Key::LeftSuper;
                break;
            case SDL_SCANCODE_RCTRL:
                return Key::RightControl;
                break;
            case SDL_SCANCODE_RSHIFT:
                return Key::RightShift;
                break;
            case SDL_SCANCODE_RALT:
                return Key::RightAlt;
                break;
            case SDL_SCANCODE_RGUI:
                return Key::RightSuper;
                break;
            case SDL_SCANCODE_MODE:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_AUDIONEXT:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_AUDIOPREV:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_AUDIOSTOP:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_AUDIOPLAY:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_AUDIOMUTE:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_MEDIASELECT:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_WWW:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_MAIL:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_CALCULATOR:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_COMPUTER:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_AC_SEARCH:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_AC_HOME:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_AC_BACK:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_AC_FORWARD:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_AC_STOP:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_AC_REFRESH:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_AC_BOOKMARKS:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_BRIGHTNESSDOWN:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_BRIGHTNESSUP:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_DISPLAYSWITCH:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KBDILLUMTOGGLE:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KBDILLUMDOWN:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_KBDILLUMUP:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_EJECT:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_SLEEP:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_APP1:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_APP2:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_AUDIOREWIND:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_AUDIOFASTFORWARD:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_SOFTLEFT:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_SOFTRIGHT:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_CALL:
                return Key::Unknown;
                break;
            case SDL_SCANCODE_ENDCALL:
                return Key::Unknown;
                break;
            case SDL_NUM_SCANCODES:
                return Key::Unknown;
                break;
                default:
                    return Key::Unknown;
                    break;
        }
    }

    MouseButton SDLWindowHandler::ConvertMouseButton(uint8_t button)
    {
        switch (button) {
            case SDL_BUTTON_LEFT:
                return MouseButton::Left;
            case SDL_BUTTON_MIDDLE:
                return MouseButton::Middle;
            case SDL_BUTTON_RIGHT:
                return MouseButton::Right;
            case SDL_BUTTON_X1:
                return MouseButton::Button4;
            case SDL_BUTTON_X2:
                return MouseButton::Button5;
            default:
                return MouseButton::Last;
        }
    }

    WindowNativeInfo SDLWindowHandler::GetNativeInfo()
    {
        WindowNativeInfo info;
#if defined(WINDOWS)
        info.window = SDL_GetProperty(SDL_GetWindowProperties(m_Window), "SDL.window.win32.hwnd", NULL);
        info.instance = SDL_GetProperty(SDL_GetWindowProperties(m_Window), "SDL.window.win32.hinstance", NULL);
#elif defined(LINUX)
        info.display = SDL_GetProperty(SDL_GetWindowProperties(m_Window), "SDL.window.x11.display", NULL);
        info.window = SDL_GetProperty(SDL_GetWindowProperties(m_Window), "SDL.window.x11.window", NULL);
#elif defined(MACOS)
        info.window = SDL_GetProperty(SDL_GetWindowProperties(m_Window), "SDL.window.cocoa.window", NULL);
#elif defined(IOS)
        info.window = SDL_GetProperty(SDL_GetWindowProperties(m_Window), "SDL.window.uikit.window", NULL);
#elif defined(ANDROID)
        info.window = SDL_GetProperty(SDL_GetWindowProperties(m_Window), "SDL.window.android.window", NULL);
#endif
        return info;
    }

    void SDLWindowHandler::InitializeDragDropOnWindows()
    {
#if defined(WINDOWS)
        HWND hwnd = (HWND)GetNativeInfo().window;
        if (FAILED(OleInitialize(NULL)))
        {
            BeeCoreError("Failed to initialize OLE");
        }
        if (FAILED(RegisterDragDrop(hwnd, static_cast<LPDROPTARGET>(new WindowsDropTarget()))))
        {
            BeeCoreError("Failed to register drop source");
        }
#endif
    }

    GlobalMouseState SDLWindowHandler::GetGlobalMouseState() const
    {
        GlobalMouseState state;
        float x, y;
        uint32_t buttons = SDL_GetGlobalMouseState(&x, &y);
        state.x = x;
        state.y = y;
        state.left = (buttons & SDL_BUTTON_LMASK) != 0;
        state.right = (buttons & SDL_BUTTON_RMASK) != 0;
        state.middle = (buttons & SDL_BUTTON_MMASK) != 0;
        return state;
    }

}
uint32_t BeeEngine::Hardware::GetSystemRAM()
{
    return SDL_GetSystemRAM();
}

BeeEngine::Hardware::SystemTheme BeeEngine::Hardware::GetSystemTheme()
{
    using enum BeeEngine::Hardware::SystemTheme;
    auto theme = SDL_GetSystemTheme();
    switch (theme)
    {
        case SDL_SYSTEM_THEME_LIGHT:
            return SystemTheme::Light;
        case SDL_SYSTEM_THEME_DARK:
            return SystemTheme::Dark;
        default:
            return SystemTheme::Unknown;
    }
}