#include "GameLayer.h"
#include <Renderer/SceneRenderer.h>

namespace BeeEngine::Runtime
{
    GameLayer::GameLayer(Ref<Scene> activeScene, Ref<FrameBuffer> frameBuffer, Locale::Domain& localeDomain)
    : m_ActiveScene(std::move(activeScene)), m_FrameBuffer(std::move(frameBuffer)), m_LocaleDomain(localeDomain)
    {}
    void GameLayer::OnUpdate(FrameData &frameData)
    {
        float32_t mouseX = Input::GetMouseX(), mouseY = Input::GetMouseY();
        ScriptingEngine::SetMousePosition(static_cast<int32_t>(mouseX), static_cast<int32_t>(mouseY));
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
    }

} // namespace BeeEngine::Runtime
