//
// Created by alexl on 01.06.2023.
//

#include "FpsCounter.h"


namespace BeeEngine::Internal
{

    void FpsCounter::Update()
    {
        auto now = Time::TotalTime();
        if(now - m_LastTime >= 1.0f)
        {
            m_CurrentFps = m_Fps;
            m_LastTime = now;
            m_Fps = 0;
        }

        m_Fps++;
    }

    void FpsCounter::Render()
    {
        ImGui::Begin("FPS");
        ImGui::Text("FPS: %.2f", m_CurrentFps);
        ImGui::Text("Frame time: %.3f ms", 1000.0f / m_CurrentFps);
        ImGui::End();
    }

    void FpsCounter::OnEvent(EventDispatcher &event)
    {

    }
}
