//
// Created by Александр Лебедев on 23.01.2023.
//
#if defined(DESKTOP_PLATFORM)
#include "GlfwWindowHandler.h"
#endif
#include "WindowHandler.h"
#include "Core/CodeSafety/Expects.h"


namespace BeeEngine{
    WindowHandler* WindowHandler::s_Instance = nullptr;
    gsl::not_null<WindowHandler*> WindowHandler::Create(WindowHandlerAPI api, const WindowProperties& properties, EventQueue& eventQueue)
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(properties.Width > 0 && properties.Height > 0 && properties.Title != nullptr);
        switch (api)
        {
            case WindowHandlerAPI::GLFW:
#if defined(DESKTOP_PLATFORM)
                return new GLFWWindowHandler(properties, eventQueue);
#else
            BeeCoreFatalError("GLFW is not supported on mobile platforms");
#endif
            default:
                BeeCoreFatalError("Invalid Window API");
        }
    }
}

