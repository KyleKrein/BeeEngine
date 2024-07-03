#include "GameLayer.h"
#include "Core/Events/EventImplementations.h"
#include "DebugLayer.h"
#include <Renderer/SceneRenderer.h>
#include <cstdint>

namespace BeeEngine::Runtime
{
    GameLayer::GameLayer(Ref<Scene> activeScene, Ref<FrameBuffer> frameBuffer, Locale::Domain& localeDomain)
        : m_ActiveScene(std::move(activeScene)), m_FrameBuffer(std::move(frameBuffer)), m_LocaleDomain(localeDomain)
    {
        m_ImGuiLayer = CreateRef<DebugLayer>();
    }
    void GameLayer::OnUpdate(FrameData& frameData)
    {
        m_ImGuiLayer->OnUpdate(frameData);
        float32_t mouseX = Input::GetMouseX(), mouseY = Input::GetMouseY();
        ScriptingEngine::SetMousePosition(static_cast<int32_t>(mouseX), static_cast<int32_t>(mouseY));
        m_MousePosition = {mouseX, mouseY};
        if (m_ActiveScene->IsRuntime()) [[likely]]
            m_ActiveScene->UpdateRuntime();
        auto cmd = m_FrameBuffer->Bind();
        SceneRenderer::RenderScene(*m_ActiveScene, cmd, m_LocaleDomain.GetLocale());
        m_FrameBuffer->Unbind(cmd);
        frameData.CopyFrameBufferImageToSwapchain(*m_FrameBuffer, 0);
        if (IsMouseInViewport())
        {
            Entity hovered = GetHoveredEntity();
            if (hovered != m_LastHoveredRuntime)
            {
                if (m_LastHoveredRuntime && m_LastHoveredRuntime.IsValid())
                {
                    ScriptingEngine::OnMouseLeave(m_LastHoveredRuntime.GetUUID());
                }
                m_LastHoveredRuntime = hovered;
                if (m_LastHoveredRuntime)
                {
                    ScriptingEngine::OnMouseEnter(m_LastHoveredRuntime.GetUUID());
                }
            }
        }
    }
    void GameLayer::OnEvent(EventDispatcher& e)
    {
        e.Dispatch<MouseButtonPressedEvent>(
            [this](MouseButtonPressedEvent& event)
            {
                // if (m_ActiveScene->IsRuntime())
                {
                    if (IsMouseInViewport())
                    {
                        Entity clicked = GetHoveredEntity();
                        if (clicked)
                        {
                            ScriptingEngine::OnMouseClick(clicked.GetUUID(), event.GetButton());
                        }
                    }
                }
                return false;
            });
        e.Dispatch<WindowResizeEvent>(
            [this](WindowResizeEvent& e)
            {
                m_FrameBuffer->Resize(std::max(e.GetWidthInPixels(), (uint16_t)1),
                                      std::max(e.GetHeightInPixels(), (uint16_t)1));
                m_ActiveScene->OnViewPortResize(e.GetWidthInPoints(), e.GetHeightInPoints());
                m_ViewportSize = {e.GetWidthInPoints(), e.GetHeightInPoints()};
                return false;
            });
        e.Dispatch<KeyPressedEvent>(
            [this](KeyPressedEvent& e)
            {
                if (e.GetKey() == Key::F4)
                {
                    m_RenderImGui = !m_RenderImGui;
                    auto& console = m_ImGuiLayer->GetConsole();
                    if (!console.IsOpen())
                        console.Toggle();
                    return true;
                }
                return false;
            });
        e.Dispatch<WindowCloseEvent>(
            [this](WindowCloseEvent& e)
            {
                m_ActiveScene->StopRuntime();
                return false;
            });
    }

    bool GameLayer::IsMouseInViewport()
    {
        return m_MousePosition.x >= 0 && m_MousePosition.y >= 0 && m_MousePosition.x < m_ViewportSize.x &&
               m_MousePosition.y < m_ViewportSize.y;
    }

    Entity GameLayer::GetHoveredEntity()
    {
        int mouseX = gsl::narrow_cast<int>(m_MousePosition.x * WindowHandler::GetInstance()->GetScaleFactor());
        int mouseY = gsl::narrow_cast<int>(m_MousePosition.y * WindowHandler::GetInstance()->GetScaleFactor());
        int pixelData = m_FrameBuffer->ReadPixel(1, mouseX, mouseY);
        pixelData--; // I make it -1 because entt starts from 0 and clear value for red integer in webgpu is
                     // 0 and I need to make invalid number -1 too, so in scene I make + 1
        if (pixelData == -1)
        {
            return Entity::Null;
        }
        return {EntityID{(entt::entity)pixelData}, m_ActiveScene};
    }

} // namespace BeeEngine::Runtime
