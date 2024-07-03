#pragma once
#include "DebugLayer.h"
#include "Scene/Entity.h"
#include <BeeEngine.h>
#include <Renderer/FrameBuffer.h>
#include <Scene/Scene.h>

namespace BeeEngine::Runtime
{
    class GameLayer : public Layer
    {
    public:
        GameLayer(Ref<Scene> activeScene, Ref<FrameBuffer> frameBuffer, Locale::Domain& localeDomain);

        void OnAttach() override { m_ActiveScene->StartRuntime(); }
        void OnDetach() override {}
        void OnUpdate(FrameData& frameData) override;
        void OnEvent(EventDispatcher& e) override;
        void OnGUIRendering() override
        {
            if (m_RenderImGui)
                m_ImGuiLayer->OnGUIRendering();
        }
        void SetScene(Ref<Scene> scene) { m_ActiveScene = std::move(scene); }

    private:
        bool IsMouseInViewport();
        Entity GetHoveredEntity();

    private:
        Ref<Scene> m_ActiveScene;
        Ref<FrameBuffer> m_FrameBuffer;
        Locale::Domain& m_LocaleDomain;
        Ref<Runtime::DebugLayer> m_ImGuiLayer;
        glm::vec2 m_ViewportSize;
        glm::vec2 m_MousePosition;
        Entity m_LastHoveredRuntime;
        bool m_RenderImGui = false;
    };
} // namespace BeeEngine::Runtime