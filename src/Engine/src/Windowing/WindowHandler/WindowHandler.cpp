//
// Created by Александр Лебедев on 23.01.2023.
//
#if defined(DESKTOP_PLATFORM) && defined(BEE_COMPILE_GLFW)
#include "GlfwWindowHandler.h"
#endif
#include "WindowHandler.h"
#include "SDLWindowHandler.h"
#include "Core/CodeSafety/Expects.h"
using namespace BeeEngine::Internal;

namespace BeeEngine{
    WindowHandler* WindowHandler::s_Instance = nullptr;
    WindowHandlerAPI WindowHandler::s_API = WindowHandlerAPI::SDL;
    gsl::not_null<WindowHandler*> WindowHandler::Create(WindowHandlerAPI api, const WindowProperties& properties, EventQueue& eventQueue)
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(properties.Width > 0 && properties.Height > 0 && properties.Title != nullptr);
        switch (api)
        {
            case WindowHandlerAPI::GLFW:
#if defined(DESKTOP_PLATFORM) && defined(BEE_COMPILE_GLFW)
                s_API = WindowHandlerAPI::GLFW;
                return new GLFWWindowHandler(properties, eventQueue);
#else
            BeeCoreWarn("GLFW is not supported. Switching to SDL3");
#endif
            case WindowHandlerAPI::SDL:
                s_API = WindowHandlerAPI::SDL;
                return new SDLWindowHandler(properties, eventQueue);
            default:
                BeeCoreFatalError("Invalid Window API");
        }
    }
}

