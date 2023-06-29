//
// Created by alexl on 14.05.2023.
//

#include "Time.h"


namespace BeeEngine
{
    double Time::m_TotalTime = 0.0f;
    double Time::m_DeltaTime = 1.0f/60.0f;

    void Time::Update(float currentTime)
    {
        m_DeltaTime = currentTime - m_TotalTime;
        m_TotalTime = currentTime;
    }

    void Time::Set(double deltaTime, double totalTime)
    {
        m_DeltaTime = deltaTime;
        m_TotalTime = totalTime;
    }
}
