//
// Created by Александр Лебедев on 07.05.2023.
//
#include "Layer.h"
#include "Windowing/WindowHandler/WindowHandler.h"
#include "Renderer/ShaderLibrary.h"
#include "Platform/ImGui/ImGuiControllerVulkan.h"

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
            case OpenGL:
                s_Controller.reset(new ImGuiControllerOpenGL());
                break;
            case Vulkan:
                s_Controller.reset(new ImGuiControllerVulkan());
                break;
            default:
                BeeCoreAssert(false, "Renderer API not supported!");
                break;
        }
        s_Controller->Initialize(WindowHandler::GetInstance()->GetWidth(),
                                 WindowHandler::GetInstance()->GetHeight(),
                                 WindowHandler::GetInstance()->GetWindow());
    }

    void Layer::AddShader(const Ref<Shader> &shader) const
    {
        ShaderLibrary::GetInstance().Add(shader);
    }

    void Layer::AddShader(std::string_view name, const Ref<Shader> &shader) const
    {
        ShaderLibrary::GetInstance().Add(name, shader);
    }

    Ref<Shader> Layer::LoadShader(std::string_view filepath) const
    {
        ShaderLibrary::GetInstance().Load(filepath);
    }

    Ref<Shader> Layer::LoadShader(std::string_view name, std::string_view filepath) const
    {
        ShaderLibrary::GetInstance().Load(name, filepath);
    }

    Ref<Shader> Layer::GetShader(std::string_view name) const
    {
        return ShaderLibrary::GetInstance().Get(name);
    }

    bool Layer::ShaderExists(std::string_view name) const
    {
        return ShaderLibrary::GetInstance().Exists(name);
    }
}