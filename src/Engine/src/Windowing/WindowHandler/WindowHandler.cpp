//
// Created by Александр Лебедев on 23.01.2023.
//
#include "GlfwWindowHandler.h"
#include "WindowHandler.h"
#include "Debug/DebugUtils.h"


namespace BeeEngine{
    WindowHandler* WindowHandler::s_Instance = nullptr;
    gsl::not_null<WindowHandler*> WindowHandler::Create(WindowHandlerAPI api, const WindowProperties& properties, EventQueue& eventQueue)
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(properties.Width > 0 && properties.Height > 0 && properties.Title != nullptr);
        switch (api)
        {
            case WindowHandlerAPI::GLFW:
                return new GLFWWindowHandler(properties, eventQueue);
            default:
                BeeCoreFatalError("Invalid Window API");
        }
    }
}

