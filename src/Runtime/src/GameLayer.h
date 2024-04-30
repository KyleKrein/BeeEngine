#pragma once
#include <BeeEngine.h>
#include <Renderer/FrameBuffer.h>
#include <Scene/Scene.h>
#include "DebugLayer.h"

namespace BeeEngine::Runtime
{
    class GameLayer : public Layer
    {
    public:
        GameLayer(Ref<Scene> activeScene, Ref<FrameBuffer> frameBuffer, Locale::Domain& localeDomain);

        void OnAttach() override
        {
            m_ActiveScene->StartRuntime();
        }
        void OnDetach() override
        {
            
        }
        void OnUpdate(FrameData& frameData) override;
        void OnEvent(EventDispatcher& e) override;
        void OnGUIRendering() override
        {
            if(m_RenderImGui)
                m_ImGuiLayer->OnGUIRendering();
        }
    private:
        Ref<Scene> m_ActiveScene;
        Ref<FrameBuffer> m_FrameBuffer;
        Locale::Domain& m_LocaleDomain;
        Ref<Runtime::DebugLayer> m_ImGuiLayer;
        bool m_RenderImGui = false;
    };
}