#pragma once

#include "glad/glad.h"
#if defined(DESKTOP_PLATFORM)
#include "GLFW/glfw3.h"
#endif
#include "WindowHandler.h"
#include "KeyCodes.h"

namespace BeeEngine
{
    class GLFWWindowHandler: public WindowHandler
    {
    public:
        GLFWWindowHandler(const WindowProperties& properties, EventQueue& eventQueue);
        GLFWWindowHandler(const GLFWWindowHandler&) = delete;
        GLFWWindowHandler& operator=(const GLFWWindowHandler&) = delete;
        virtual void SetWidth(uint16_t width) override;
        virtual void SetHeight(uint16_t height) override;
        virtual uint64_t GetWindow() override;

        void SetVSync(VSync mode) override;

        void HideCursor() override;

        void DisableCursor() override;

        void ShowCursor() override;

        void ProcessEvents() override;

        void SwapBuffers() override;

        void MakeContextCurrent() override;

        void MakeContextNonCurrent() override;

        [[nodiscard]] bool IsRunning() const override;

        void UpdateTime() override;

        void Close() override;

        GraphicsDevice& GetGraphicsDevice() override
        {
            return *m_GraphicsDevice;
        }
        Instance& GetAPIInstance() override
        {
            return *m_Instance;
        }

        ~GLFWWindowHandler() override;
    private:
        static Key ConvertKeyCode(int key);
#if defined(DESKTOP_PLATFORM)
        //Callbacks
        static void CharCallback(GLFWwindow* window, unsigned int codepoint);
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
        static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
        static void WindowSizeCallback(GLFWwindow* window, int width, int height);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void WindowCloseCallback(GLFWwindow* window);
#endif
    private:
#if defined(DESKTOP_PLATFORM)
        struct glfwFinalizer
        {
            GLFWwindow *window;
            ~glfwFinalizer()
            {
                glfwDestroyWindow(window);
                glfwTerminate();
            }
        };
        glfwFinalizer m_Finalizer;
        GLFWwindow* m_Window;
#endif

        mutable bool m_IsRunning;
        mutable bool m_IsClosing;

        Scope<Instance> m_Instance;
        Scope<GraphicsDevice> m_GraphicsDevice;

        void InitializeOpenGL(const WindowProperties &properties);

        void InitializeVulkan(const WindowProperties &properties);
    };
}


