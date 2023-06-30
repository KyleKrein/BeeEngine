#pragma once

#include "Core/Events/Event.h"
#include "Platform/ImGui/ImGuiController.h"
#include "Renderer/Shader.h"
#include "Debug/Instrumentor.h"


namespace BeeEngine
{
    class Layer
    {
    public:
        virtual ~Layer() = default;
        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate() {}
        virtual void OnGUIRendering() {}
        virtual void OnEvent(EventDispatcher& e) {}
    protected:
        void AddShader(const Ref<Shader>& shader) const;
        void AddShader(std::string_view name, const Ref<Shader>& shader) const;
        [[nodiscard]] Ref<Shader> LoadShader(std::string_view filepath) const;
        [[nodiscard]] Ref<Shader> LoadShader(std::string_view name, std::string_view filepath) const;

        [[nodiscard]] Ref<Shader> GetShader(std::string_view name) const;
        [[nodiscard]] bool ShaderExists(std::string_view name) const;
    };

    class ImGuiLayer: public Layer
    {
    public:
        ImGuiLayer()
        {
            BEE_PROFILE_FUNCTION();
            Init();
        }
        void OnBegin()
        {
            BEE_PROFILE_FUNCTION();
            s_Controller->Update();
        };
        void OnEnd()
        {
            BEE_PROFILE_FUNCTION();
            s_Controller->Render();
        };
        void OnDetach() override
        {
            BEE_PROFILE_FUNCTION();
            s_Controller->Shutdown();
        }
        virtual void OnGUIRendering() override
        {

        }
        static void Init();
    private:
        static Scope<ImGuiController> s_Controller;
    };
}