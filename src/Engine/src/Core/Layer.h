#pragma once

#include "Core/Events/Event.h"
#include "Platform/ImGui/ImGuiController.h"
#include "Debug/Instrumentor.h"
#include "Core/AssetManagement/IAssetManager.h"
#include "Core/AssetManagement/AssetManager.h"
#include "Renderer/Renderer.h"


namespace BeeEngine
{
    class Layer
    {
    public:
        virtual ~Layer() = default;
        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(FrameData& frameData) {}
        virtual void OnGUIRendering() {}
        virtual void OnEvent(EventDispatcher& e) {}
    protected:
        void RegisterAssetManager(IAssetManager* assetManager)
        {
            AssetManager::s_AssetManager = assetManager;
        }
    };

    class ImGuiLayer final: public Layer
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
        void OnEnd(CommandBuffer& cmd)
        {
            BEE_PROFILE_FUNCTION();
            s_Controller->Render(cmd);
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
