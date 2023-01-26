#pragma once

#include "WindowHandler.h"
#include "GLFW/glfw3.h"

namespace BeeEngine
{
    class GLFWWindowHandler: public WindowHandler
    {
    public:
        GLFWWindowHandler(const WindowProperties& properties);
        virtual void SetWidth(uint16_t width) override;
        virtual void SetHeight(uint16_t height) override;
        ~GLFWWindowHandler() override;
    private:
        GLFWwindow* m_Window;
    };
}


