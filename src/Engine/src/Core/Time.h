//
// Created by alexl on 14.05.2023.
//

#pragma once
#include <chrono>
#include "Core/Numbers.h"
namespace BeeEngine
{
    class Time
    {
    public:
        using millisecondsD = std::chrono::duration<float64_t, std::milli>;
        using secondsD = std::chrono::duration<float64_t, std::ratio<1, 1>>;
        inline static secondsD TotalTime() { return m_TotalTime; }
        inline static secondsD DeltaTime() { return m_DeltaTime; }
        inline static secondsD AverageDeltaTime() { return m_AverageDeltaTime; }

        friend class WindowHandler;
    private:

        static void Update(secondsD currentTime);
        static void Set(secondsD deltaTime, secondsD totalTime);
        constexpr static secondsD SumFrameTimes();
        static secondsD m_TotalTime;
        static secondsD m_DeltaTime;
        static secondsD m_AverageDeltaTime;

        static constexpr uint32_t m_FrameSamples = 5; //For now 5 frames are enough to calculate average delta time
        static Time::secondsD m_FrameTimes[m_FrameSamples];
        static uint32_t m_FrameIndex;

        static void CalculateAverageDeltaTime();
    };
}
