#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "WindowHandler.h"
#include "KeyCodes.h"

namespace BeeEngine
{
    class GLFWWindowHandler: public WindowHandler
    {
    public:
        GLFWWindowHandler(const WindowProperties& properties, EventQueue& eventQueue);
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

        bool IsRunning() const override;

        void UpdateTime() override;

        ~GLFWWindowHandler() override;
    private:
        static Key ConvertKeyCode(int key);
        //Callbacks
        static void CharCallback(GLFWwindow* window, unsigned int codepoint);
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
        static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void WindowCloseCallback(GLFWwindow* window);
    private:
        GLFWwindow* m_Window;
        bool m_IsRunning;
    };
}


