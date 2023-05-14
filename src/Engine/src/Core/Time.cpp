//
// Created by alexl on 14.05.2023.
//

#include "Time.h"


namespace BeeEngine
{
    float Time::m_TotalTime = 0.0f;
    float Time::m_DeltaTime = 1.0f/60.0f;

    void Time::Update(float currentTime)
    {
        m_DeltaTime = currentTime - m_TotalTime;
        m_TotalTime = currentTime;
    }
}
