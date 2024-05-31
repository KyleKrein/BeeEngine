#include "GameLayer.h"
#include <Renderer/SceneRenderer.h>
#include "DebugLayer.h"

namespace BeeEngine::Runtime
{
    GameLayer::GameLayer(Ref<Scene> activeScene, Ref<FrameBuffer> frameBuffer, Locale::Domain& localeDomain)
    : m_ActiveScene(std::move(activeScene)), m_FrameBuffer(std::move(frameBuffer)), m_LocaleDomain(localeDomain)
    {
        m_ImGuiLayer = CreateRef<DebugLayer>();
    }
    void GameLayer::OnUpdate(FrameData &frameData)
    {
        m_ImGuiLayer->OnUpdate(frameData);
        float32_t mouseX = Input::GetMouseX(), mouseY = Input::GetMouseY();
        ScriptingEngine::SetMousePosition(static_cast<int32_t>(mouseX), static_cast<int32_t>(mouseY));
        if(m_ActiveScene->IsRuntime()) [[likely]]
            m_ActiveScene->UpdateRuntime();
        auto cmd = m_FrameBuffer->Bind();
        SceneRenderer::RenderScene(*m_ActiveScene, cmd, m_LocaleDomain.GetLocale());
        m_FrameBuffer->Unbind(cmd);
        frameData.CopyFrameBufferImageToSwapchain(*m_FrameBuffer, 0);
    }
    void GameLayer::OnEvent(EventDispatcher& e)
    {
        e.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e)
        {
            m_FrameBuffer->Resize(e.GetWidthInPixels(), e.GetHeightInPixels());
            m_ActiveScene->OnViewPortResize(e.GetWidthInPoints(), e.GetHeightInPoints());
            return false;
        });
        e.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& e)
        {
            if (e.GetKey() == Key::F4)
            {
                m_RenderImGui = !m_RenderImGui;
                auto& console = m_ImGuiLayer->GetConsole();
                if(!console.IsOpen())
                    console.Toggle();
                return true;
            }
            return false;
        });
        e.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e)
        {
            m_ActiveScene->StopRuntime();
            return false;
        });
    }

} // namespace BeeEngine::Runtime
