//
// Created by alexl on 01.06.2023.
//

#pragma once

#include "IImGuiElement.h"

namespace BeeEngine::Internal
{
    class FpsCounter final: public IImGuiElement
    {
    public:
        void Update() override;
        void Render() override;
        void OnEvent(EventDispatcher& event) override;
    private:
        float m_LastTime = Time::TotalTime();
        float m_Fps = 0;
        float m_CurrentFps = 0;
    };
}
