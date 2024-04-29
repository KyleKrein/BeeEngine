#include <BeeEngine.h>
#include <Gui/ImGui/FpsCounter.h>
#include <Gui/ImGui/RendererStatisticsGUI.h>

namespace BeeEngine::Runtime
{
    class DebugLayer : public Layer
    {
    public:
        void OnUpdate(FrameData& frameData) override
        {
            m_FpsCounter.Update();
        }
        void OnGUIRendering() override
        {
            m_FpsCounter.Render();
            m_RendererStatisticsGUI.Render();
        }
    private:
        BeeEngine::Internal::FpsCounter m_FpsCounter {};
        BeeEngine::Internal::RendererStatisticsGUI m_RendererStatisticsGUI {};
    };
}