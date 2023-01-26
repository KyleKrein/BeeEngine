//
// Created by Александр Лебедев on 23.01.2023.
//

#include "GlfwWindowHandler.h"
#include "GLFW/glfw3.h"
#include "Core/Logging/Log.h"

namespace BeeEngine
{

    GLFWWindowHandler::GLFWWindowHandler(const WindowProperties &properties)
    {
        m_Width = properties.Width;
        m_Height = properties.Height;
        m_Title = properties.Title;
        BeeAssert(glfwInit(), "GLFW could not me initialized!");
        m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, nullptr, nullptr);
        
    }

    void GLFWWindowHandler::SetWidth(uint16_t width)
    {

    }

    void GLFWWindowHandler::SetHeight(uint16_t height)
    {

    }

    GLFWWindowHandler::~GLFWWindowHandler()
    {

    }
}

