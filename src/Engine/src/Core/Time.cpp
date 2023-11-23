//
// Created by alexl on 14.05.2023.
//

#include "Time.h"
#include "Core/Logging/Log.h"

using namespace std::chrono_literals;
namespace BeeEngine
{
    Time::secondsD Time::m_TotalTime = 0ms;
    Time::secondsD Time::m_DeltaTime = 16ms;
    Time::secondsD Time::m_AverageDeltaTime = 16ms;
    Time::secondsD Time::m_FrameTimes[m_FrameSamples] = {0s};
    uint32_t Time::m_FrameIndex = 0;

    constexpr Time::secondsD Time::SumFrameTimes()
    {
        Time::secondsD sum = 0ms;
        for (auto& time : Time::m_FrameTimes)
        {
            sum += time;
        }
        return sum;
    }

    void Time::Update(Time::secondsD currentTime)
    {
        Time::secondsD deltaTime = currentTime - m_TotalTime;
        Set(deltaTime, currentTime);
    }

    void Time::CalculateAverageDeltaTime()
    {
        if (m_FrameIndex == m_FrameSamples)
        {
            m_FrameIndex = 0;
            m_AverageDeltaTime = SumFrameTimes() / m_FrameSamples;
        }
        else
        {
            m_FrameTimes[m_FrameIndex++] = m_DeltaTime;
        }
    }

    void Time::Set(Time::secondsD deltaTime, Time::secondsD totalTime)
    {
        m_DeltaTime = deltaTime;
        m_TotalTime = totalTime;
#if defined(DEBUG)
        if (m_DeltaTime > 10s)
        {
            BeeCoreWarn("DeltaTime is too big: {0} seconds", m_DeltaTime.count());
            m_DeltaTime = 16ms;
        }
#endif
        CalculateAverageDeltaTime();
    }
}
