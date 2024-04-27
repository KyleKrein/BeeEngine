using System;

namespace BeeEngine
{
    public static class Time
    {
        public static float DeltaTime => (float)m_DeltaTime;
        public static float TotalTime => (float)m_TotalTime;
        public static double DeltaTimeD => m_DeltaTime;
        public static double TotalTimeD => m_TotalTime;
        public static TimeSpan DeltaTimeSpan => TimeSpan.FromSeconds(m_DeltaTime);

        public static TimeSpan TotalTimeSpan => TimeSpan.FromSeconds(m_TotalTime);

        //Both are in seconds
        private static double m_DeltaTime = 1d / 60d;
        private static double m_TotalTime = 0d;
    }
}