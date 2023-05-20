#pragma once

#include "Core/Events/Event.h"
#include "Platform/ImGui/ImGuiLoaderOpenGL.h"
#include "Platform/ImGui/ImGuiController.h"
#include "Renderer/Renderer.h"
#include "Allocator/Allocator.h"


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
        virtual void OnGUIRendering() override
        {

        }
        static void Init();
    private:
        static ImGuiController* s_Controller;
    };
}