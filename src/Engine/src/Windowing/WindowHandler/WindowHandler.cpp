//
// Created by Александр Лебедев on 23.01.2023.
//
#if defined(DESKTOP_PLATFORM) && defined(BEE_COMPILE_GLFW)
#include "GlfwWindowHandler.h"
#endif
#include "WindowHandler.h"
#include "SDLWindowHandler.h"
#include "WinAPIWindowHandler.h"
#include "Core/CodeSafety/Expects.h"
#include "Debug/Instrumentor.h"
using namespace BeeEngine::Internal;

namespace BeeEngine{
    WindowHandler* WindowHandler::s_Instance = nullptr;
    WindowHandlerAPI WindowHandler::s_API = WindowHandlerAPI::SDL;
    gsl::not_null<WindowHandler*> WindowHandler::Create(WindowHandlerAPI api, const ApplicationProperties& properties, EventQueue& eventQueue)
    {
        BEE_PROFILE_FUNCTION();
        BeeExpects(properties.WindowWidth > 0 && properties.WindowHeight > 0 && properties.Title != nullptr);
        s_API = api;
        switch (api)
        {
            case WindowHandlerAPI::SDL:
#if defined(BEE_COMPILE_SDL)
                return new SDLWindowHandler(properties, eventQueue);
#endif
#if defined(WINDOWS)
            case WindowHandlerAPI::WinAPI:
                return new WinAPIWindowHandler(properties, eventQueue);
#endif
            default:
                BeeCoreFatalError("Invalid Window API");
        }
    }
}

