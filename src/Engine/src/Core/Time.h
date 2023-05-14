//
// Created by alexl on 14.05.2023.
//

#pragma once

namespace BeeEngine
{
    class Time
    {
    public:
        inline static float TotalTime() { return m_TotalTime; }
        inline static float DeltaTime() { return m_DeltaTime; }

        friend class WindowHandler;
    private:

        static void Update(float currentTime);
        static float m_TotalTime;
        static float m_DeltaTime;
    };
}
