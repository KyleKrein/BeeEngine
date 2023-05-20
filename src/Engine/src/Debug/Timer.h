//
// Created by alexl on 20.05.2023.
//

#pragma once
#include "chrono"
namespace BeeEngine
{
    namespace Debug
    {
        class Timer
        {
        public:
            Timer()
            {
                m_StartTime = std::chrono::high_resolution_clock::now();
            }
            ~Timer();

            void Start();
            void Stop()
            {
                auto stopTime = std::chrono::high_resolution_clock::now();
                auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTime).time_since_epoch().count();
                auto end = std::chrono::time_point_cast<std::chrono::microseconds>(stopTime).time_since_epoch().count();
                auto duration = end - start;
            }
            void Reset();

            double GetTime();
        private:
            std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
        };
    }
}