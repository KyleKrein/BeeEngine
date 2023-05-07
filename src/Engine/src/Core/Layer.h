#pragma once

#include "Core/Events/Event.h"
#include "Platform/ImGui/ImGuiLoaderOpenGL.h"
#include "Platform/ImGui/ImGuiController.h"
#include "Renderer/Renderer.h"


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
    };

    class ImGuiLayer: public Layer
    {
    public:
        ImGuiLayer()
        {
            Init();
        }
        void OnBegin()
        {
            s_Controller->Update();
        };
        void OnEnd()
        {
            s_Controller->Render();
        };
        virtual void OnGUIRendering() override
        {
            ImGui::ShowDemoWindow();
        }
        static void Init();
    private:
        static ImGuiController* s_Controller;
    };
}