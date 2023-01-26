//
// Created by Александр Лебедев on 23.01.2023.
//

#include "WindowHandler.h"
#include "GlfwWindowHandler.h"

namespace BeeEngine{
    WindowHandler* WindowHandler::Create(WindowHandlerAPI api, const WindowProperties& properties)
    {
        switch (api)
        {
            case WindowHandlerAPI::GLFW:
                return new GLFWWindowHandler(properties)
        }
    }
}

