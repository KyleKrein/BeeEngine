//
// Created by alexl on 14.05.2023.
//

#pragma once

namespace BeeEngine
{
    class Time
    {
    public:
        inline static double TotalTime() { return m_TotalTime; }
        inline static double DeltaTime() { return m_DeltaTime; }

        friend class WindowHandler;
    private:

        static void Update(double currentTime);
        static void Set(double deltaTime, double totalTime);
        static double m_TotalTime;
        static double m_DeltaTime;
    };
}
