//
// Created by Александр Лебедев on 23.01.2023.
//
#include "GlfwWindowHandler.h"
#include "WindowHandler.h"


namespace BeeEngine{
    WindowHandler* WindowHandler::s_Instance = nullptr;
    WindowHandler* WindowHandler::Create(WindowHandlerAPI api, const WindowProperties& properties, EventQueue& eventQueue)
    {
        switch (api)
        {
            case WindowHandlerAPI::GLFW:
                return new GLFWWindowHandler(properties, eventQueue);
            default:
                BeeCoreError("Invalid Window API");
                return nullptr;
        }
    }
}

