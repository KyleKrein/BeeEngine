#include "Window.h"
#include "Core/Application.h"
#include "CrossPlatformWindow.h"

namespace BeeEngine{

    Window* Window::Create(const WindowProperties& properties, EventQueue& eventQueue)
    {
        switch (Application::GetOsPlatform())
        {
            case OSPlatform::Windows:
                return new CrossPlatformWindow(properties, eventQueue);
            case OSPlatform::Linux:
                return new CrossPlatformWindow(properties, eventQueue);
            case OSPlatform::Mac:
                return new CrossPlatformWindow(properties, eventQueue);
            case OSPlatform::iOS:
                return new CrossPlatformWindow(properties, eventQueue);
            case OSPlatform::Android:
                return new CrossPlatformWindow(properties, eventQueue);
            default:
                BeeCoreError("Unsupported platform");
                return nullptr;
        }
    }
}