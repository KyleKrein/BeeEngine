//
// Created by Александр Лебедев on 23.01.2023.
//

#include "GlfwWindowHandler.h"
#include "Core/Logging/Log.h"
#include "Core/Application.h"
#include "vulkan/vulkan.hpp"
#include "Platform/Vulkan/VulkanInstance.h"
#include "Platform/Vulkan/VulkanGraphicsDevice.h"


namespace BeeEngine
{

    GLFWWindowHandler::GLFWWindowHandler(const WindowProperties &properties, EventQueue &eventQueue)
            : WindowHandler(eventQueue), m_Window(nullptr), m_IsRunning(false), m_IsClosing(false)
    {
        BEE_PROFILE_FUNCTION();
        s_Instance = this;
        m_Title = properties.Title;
        m_vsync = properties.Vsync;
        glfwSetErrorCallback([](int error_code, const char *description)
                             {
                                 BeeCoreError("GLFW failure! Code {0}. Description: {1}", error_code, description);
                             });
        BeeCoreAssert(glfwInit(), "GLFW could not me initialized!");
        switch (properties.PreferredRenderAPI)
        {
            case RenderAPI::OpenGL:
                InitializeOpenGL(properties);
                break;
            case RenderAPI::Vulkan:
                InitializeVulkan(properties);
                break;
            default:
                BeeCoreError("Unknown RenderAPI");
                break;
        }

        m_IsRunning = true;

        //SET CALLBACKS
        glfwSetCharCallback(m_Window, CharCallback);
        glfwSetKeyCallback(m_Window, KeyCallback);
        glfwSetScrollCallback(m_Window, ScrollCallback);
        glfwSetCursorPosCallback(m_Window, CursorPosCallback);
        glfwSetFramebufferSizeCallback(m_Window, FrameBufferSizeCallback);
        glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);
        glfwSetWindowCloseCallback(m_Window, WindowCloseCallback);

        //glViewport(0, 0, m_Width, m_Height);
    }

    void GLFWWindowHandler::SetWidth(uint16_t width)
    {
        BEE_PROFILE_FUNCTION();
        if (Application::GetOsPlatform() == OSPlatform::Mac)
        {
            m_Width = width/2;
        }
        else
        {
            m_Width = width;
        }
        glfwSetWindowSize(m_Window, m_Width, m_Height);
    }

    void GLFWWindowHandler::SetHeight(uint16_t height)
    {
        BEE_PROFILE_FUNCTION();
        if (Application::GetOsPlatform() == OSPlatform::Mac)
        {
            m_Height = height/2;
        }
        else
        {
            m_Height = height;
        }
        glfwSetWindowSize(m_Window, m_Width, m_Height);
    }

    GLFWWindowHandler::~GLFWWindowHandler()
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void GLFWWindowHandler::SetVSync(VSync mode)
    {
        BEE_PROFILE_FUNCTION();
        m_vsync = mode;
        glfwSwapInterval(mode);
    }

    void GLFWWindowHandler::HideCursor()
    {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    void GLFWWindowHandler::DisableCursor()
    {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void GLFWWindowHandler::ShowCursor()
    {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void GLFWWindowHandler::ProcessEvents()
    {
        glfwPollEvents();
    }

    void GLFWWindowHandler::SwapBuffers()
    {
        glfwSwapBuffers(m_Window);
    }

    void GLFWWindowHandler::MakeContextCurrent()
    {
        BEE_PROFILE_FUNCTION();
        glfwMakeContextCurrent(m_Window);
    }

    void GLFWWindowHandler::MakeContextNonCurrent()
    {
        BEE_PROFILE_FUNCTION();
        glfwMakeContextCurrent(nullptr);
    }

    bool GLFWWindowHandler::IsRunning() const
    {
        if(m_IsClosing) [[unlikely]]
        {
            m_IsRunning = false;
            m_IsClosing = false;
            return true;
        }
        return m_IsRunning;
    }

    void GLFWWindowHandler::CharCallback(GLFWwindow *window, unsigned int codepoint)
    {
        auto event = CreateScope<CharTypedEvent>(codepoint);
        ((GLFWWindowHandler*)s_Instance)->m_Events.AddEvent(std::move(event));
    }

    void GLFWWindowHandler::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        auto event = CreateScope<KeyPressedEvent>(ConvertKeyCode(key));
        ((GLFWWindowHandler*)s_Instance)->m_Events.AddEvent(std::move(event));
    }

    void GLFWWindowHandler::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
    {
        auto event = CreateScope<MouseScrolledEvent>(xoffset, yoffset);
        ((GLFWWindowHandler*)s_Instance)->m_Events.AddEvent(std::move(event));
    }

    void GLFWWindowHandler::CursorPosCallback(GLFWwindow *window, double xpos, double ypos)
    {
        auto event = CreateScope<MouseMovedEvent>(xpos, ypos);
        ((GLFWWindowHandler*)s_Instance)->m_Events.AddEvent(std::move(event));
    }

    void GLFWWindowHandler::FrameBufferSizeCallback(GLFWwindow *window, int width, int height)
    {
        auto event = CreateScope<WindowResizeEvent>(width, height);
        ((GLFWWindowHandler*)s_Instance)->m_Events.AddEvent(std::move(event));
    }

    void GLFWWindowHandler::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
    {
        auto event = CreateScope<MouseButtonPressedEvent>((MouseButton)button);
        ((GLFWWindowHandler*)s_Instance)->m_Events.AddEvent(std::move(event));
    }

    void GLFWWindowHandler::WindowCloseCallback(GLFWwindow *window)
    {
        auto event = CreateScope<WindowCloseEvent>();
        ((GLFWWindowHandler*)s_Instance)->m_Events.AddEvent(std::move(event));
        ((GLFWWindowHandler*)s_Instance)->m_IsClosing = true;
    }

    Key GLFWWindowHandler::ConvertKeyCode(int key)
    {
        switch (key)
        {
            case GLFW_KEY_UNKNOWN:
                return Key::Unknown;
            case GLFW_KEY_SPACE:
                return Key::Space;
            case GLFW_KEY_APOSTROPHE:
                return Key::Apostrophe;
            case GLFW_KEY_COMMA:
                return Key::Comma;
            case GLFW_KEY_MINUS:
                return Key::Minus;
            case GLFW_KEY_PERIOD:
                return Key::Period;
            case GLFW_KEY_SLASH:
                return Key::Slash;
            case GLFW_KEY_0:
                return Key::D0;
            case GLFW_KEY_1:
                return Key::D1;
            case GLFW_KEY_2:
                return Key::D2;
            case GLFW_KEY_3:
                return Key::D3;
            case GLFW_KEY_4:
                return Key::D4;
            case GLFW_KEY_5:
                return Key::D5;
            case GLFW_KEY_6:
                return Key::D6;
            case GLFW_KEY_7:
                return Key::D7;
            case GLFW_KEY_8:
                return Key::D8;
            case GLFW_KEY_9:
                return Key::D9;
            case GLFW_KEY_SEMICOLON:
                return Key::Semicolon;
            case GLFW_KEY_EQUAL:
                return Key::Equal;
            case GLFW_KEY_A:
                return Key::A;
            case GLFW_KEY_B:
                return Key::B;
            case GLFW_KEY_C:
                return Key::C;
            case GLFW_KEY_D:
                return Key::D;
            case GLFW_KEY_E:
                return Key::E;
            case GLFW_KEY_F:
                return Key::F;
            case GLFW_KEY_G:
                return Key::G;
            case GLFW_KEY_H:
                return Key::H;
            case GLFW_KEY_I:
                return Key::I;
            case GLFW_KEY_J:
                return Key::J;
            case GLFW_KEY_K:
                return Key::K;
            case GLFW_KEY_L:
                return Key::L;
            case GLFW_KEY_M:
                return Key::M;
            case GLFW_KEY_N:
                return Key::N;
            case GLFW_KEY_O:
                return Key::O;
            case GLFW_KEY_P:
                return Key::P;
            case GLFW_KEY_Q:
                return Key::Q;
            case GLFW_KEY_R:
                return Key::R;
            case GLFW_KEY_S:
                return Key::S;
            case GLFW_KEY_T:
                return Key::T;
            case GLFW_KEY_U:
                return Key::U;
            case GLFW_KEY_V:
                return Key::V;
            case GLFW_KEY_W:
                return Key::W;
            case GLFW_KEY_X:
                return Key::X;
            case GLFW_KEY_Y:
                return Key::Y;
            case GLFW_KEY_Z:
                return Key::Z;
            case GLFW_KEY_LEFT_BRACKET:
                return Key::LeftBracket;
            case GLFW_KEY_BACKSLASH:
                return Key::Backslash;
            case GLFW_KEY_RIGHT_BRACKET:
                return Key::RightBracket;
            case GLFW_KEY_GRAVE_ACCENT:
                return Key::GraveAccent;
            case GLFW_KEY_ESCAPE:
                return Key::Escape;
            case GLFW_KEY_ENTER:
                return Key::Enter;
            case GLFW_KEY_TAB:
                return Key::Tab;
            case GLFW_KEY_BACKSPACE:
                return Key::Backspace;
            case GLFW_KEY_INSERT:
                return Key::Insert;
            case GLFW_KEY_DELETE:
                return Key::Delete;
            case GLFW_KEY_RIGHT:
                return Key::Right;
            case GLFW_KEY_LEFT:
                return Key::Left;
            case GLFW_KEY_DOWN:
                return Key::Down;
            case GLFW_KEY_UP:
                return Key::Up;
            case GLFW_KEY_PAGE_UP:
                return Key::PageUp;
            case GLFW_KEY_PAGE_DOWN:
                return Key::PageDown;
            case GLFW_KEY_HOME:
                return Key::Home;
            case GLFW_KEY_END:
                return Key::End;
            case GLFW_KEY_CAPS_LOCK:
                return Key::CapsLock;
            case GLFW_KEY_SCROLL_LOCK:
                return Key::ScrollLock;
            case GLFW_KEY_NUM_LOCK:
                return Key::NumLock;
            case GLFW_KEY_PRINT_SCREEN:
                return Key::PrintScreen;
            case GLFW_KEY_PAUSE:
                return Key::Pause;
            case GLFW_KEY_F1:
                return Key::F1;
            case GLFW_KEY_F2:
                return Key::F2;
            case GLFW_KEY_F3:
                return Key::F3;
            case GLFW_KEY_F4:
                return Key::F4;
            case GLFW_KEY_F5:
                return Key::F5;
            case GLFW_KEY_F6:
                return Key::F6;
            case GLFW_KEY_F7:
                return Key::F7;
            case GLFW_KEY_F8:
                return Key::F8;
            case GLFW_KEY_F9:
                return Key::F9;
            case GLFW_KEY_F10:
                return Key::F10;
            case GLFW_KEY_F11:
                return Key::F11;
            case GLFW_KEY_F12:
                return Key::F12;
            case GLFW_KEY_F13:
                return Key::F13;
            case GLFW_KEY_F14:
                return Key::F14;
            case GLFW_KEY_F15:
                return Key::F15;
            case GLFW_KEY_F16:
                return Key::F16;
            case GLFW_KEY_F17:
                return Key::F17;
            case GLFW_KEY_F18:
                return Key::F18;
            case GLFW_KEY_F19:
                return Key::F19;
            case GLFW_KEY_F20:
                return Key::F20;
            case GLFW_KEY_F21:
                return Key::F21;
            case GLFW_KEY_F22:
                return Key::F22;
            case GLFW_KEY_F23:
                return Key::F23;
            case GLFW_KEY_F24:
                return Key::F24;
            case GLFW_KEY_F25:
                return Key::F25;
            case GLFW_KEY_KP_0:
                return Key::KeyPad0;
            case GLFW_KEY_KP_1:
                return Key::KeyPad1;
            case GLFW_KEY_KP_2:
                return Key::KeyPad2;
            case GLFW_KEY_KP_3:
                return Key::KeyPad3;
            case GLFW_KEY_KP_4:
                return Key::KeyPad4;
            case GLFW_KEY_KP_5:
                return Key::KeyPad5;
            case GLFW_KEY_KP_6:
                return Key::KeyPad6;
            case GLFW_KEY_KP_7:
                return Key::KeyPad7;
            case GLFW_KEY_KP_8:
                return Key::KeyPad8;
            case GLFW_KEY_KP_9:
                return Key::KeyPad9;
            case GLFW_KEY_KP_DECIMAL:
                return Key::KeyPadDecimal;
            case GLFW_KEY_KP_DIVIDE:
                return Key::KeyPadDivide;
            case GLFW_KEY_KP_MULTIPLY:
                return Key::KeyPadMultiply;
            case GLFW_KEY_KP_SUBTRACT:
                return Key::KeyPadSubtract;
            case GLFW_KEY_KP_ADD:
                return Key::KeyPadAdd;
            case GLFW_KEY_KP_ENTER:
                return Key::KeyPadEnter;
            case GLFW_KEY_KP_EQUAL:
                return Key::KeyPadEqual;
            case GLFW_KEY_LEFT_SHIFT:
                return Key::LeftShift;
            case GLFW_KEY_LEFT_CONTROL:
                return Key::LeftControl;
            case GLFW_KEY_LEFT_ALT:
                return Key::LeftAlt;
            case GLFW_KEY_LEFT_SUPER:
                return Key::LeftSuper;
            case GLFW_KEY_RIGHT_SHIFT:
                return Key::RightShift;
            case GLFW_KEY_RIGHT_CONTROL:
                return Key::RightControl;
            case GLFW_KEY_RIGHT_ALT:
                return Key::RightAlt;
            case GLFW_KEY_RIGHT_SUPER:
                return Key::RightSuper;
            default:
                return Key::Unknown;
        }
    }

    uint64_t GLFWWindowHandler::GetWindow()
    {
        return (uint64_t)m_Window;
    }

    void GLFWWindowHandler::UpdateTime()
    {
        BEE_PROFILE_FUNCTION();
        UpdateDeltaTime(gsl::narrow_cast<float>(glfwGetTime()));
    }

    void GLFWWindowHandler::Close()
    {
        WindowCloseCallback(m_Window);
    }

    void GLFWWindowHandler::InitializeOpenGL(const WindowProperties &properties)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        if (Application::GetOsPlatform() == OSPlatform::Mac)
        {
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

            m_Width = properties.Width / 2;
            m_Height = properties.Height / 2;
            m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, nullptr, nullptr);
            BeeCoreAssert(m_Window, "Window initialization failed");
        } else
        {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
            m_Width = properties.Width;
            m_Height = properties.Height;

            m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, nullptr, nullptr);
            BeeCoreAssert(m_Window, "Window initialization failed");

        }
        glfwMakeContextCurrent(m_Window);
        //LOAD GLAD
        if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        {
            BeeCoreError("GLAD initialization failed!");
        } else
        {
            BeeCoreInfo("GLAD initialized successfully!");
        }
        glfwSwapInterval(properties.Vsync);
    }

    void GLFWWindowHandler::InitializeVulkan(const WindowProperties &properties)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        m_Width = properties.Width;
        m_Height = properties.Height;
        m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, nullptr, nullptr);
        BeeCoreAssert(m_Window, "Window initialization failed");

        m_Instance = CreateRef<Internal::VulkanInstance>(properties.Title, WindowHandlerAPI::GLFW);
        m_GraphicsDevice = CreateRef<Internal::VulkanGraphicsDevice>(*(Internal::VulkanInstance*)m_Instance.get());
    }
}

