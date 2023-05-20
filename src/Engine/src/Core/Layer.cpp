//
// Created by Александр Лебедев on 07.05.2023.
//
#include "Layer.h"
#include "Windowing/WindowHandler/WindowHandler.h"

namespace BeeEngine
{
    ImGuiController* ImGuiLayer::s_Controller = nullptr;

    void ImGuiLayer::Init()
    {
        BEE_PROFILE_FUNCTION();
        if (s_Controller != nullptr)
        {
            return;
        }
        switch (Renderer::GetAPI())
        {
            case OpenGL:
                s_Controller = new ImGuiControllerOpenGL();
                break;
            default:
                BeeCoreAssert(false, "Renderer API not supported!");
                break;
        }
        s_Controller->Initialize(WindowHandler::GetInstance()->GetWidth(),
                                 WindowHandler::GetInstance()->GetHeight(),
                                 WindowHandler::GetInstance()->GetWindow());
    }
}