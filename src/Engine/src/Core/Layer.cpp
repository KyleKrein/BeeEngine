//
// Created by Александр Лебедев on 07.05.2023.
//
#include "Layer.h"

#include "Platform/ImGui/ImGuiControllerVulkan.h"
#include "Platform/ImGui/ImGuiControllerWebGPU.h"
#include "Renderer/Renderer.h"
#include "Windowing/WindowHandler/WindowHandler.h"
#include <memory>

namespace BeeEngine
{
    Scope<ImGuiController> ImGuiLayer::s_Controller = nullptr;

    void ImGuiLayer::Init()
    {
        BEE_PROFILE_FUNCTION();
        if (s_Controller != nullptr)
        {
            return;
        }
        switch (Renderer::GetAPI())
        {
#if defined(BEE_COMPILE_WEBGPU)
            case WebGPU:
                s_Controller = std::make_unique<Internal::ImGuiControllerWebGPU>();
                break;
#endif
#if defined(BEE_COMPILE_VULKAN)
            case Vulkan:
                s_Controller = std::make_unique<Internal::ImGuiControllerVulkan>();
                break;
#endif
            default:
                BeeCoreAssert(false, "Renderer API not supported!");
                break;
        }
        s_Controller->Initialize(WindowHandler::GetInstance()->GetWidth(),
                                 WindowHandler::GetInstance()->GetHeight(),
                                 WindowHandler::GetInstance()->GetWindow());
    }
} // namespace BeeEngine