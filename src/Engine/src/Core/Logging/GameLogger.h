//
// Created by alexl on 22.07.2023.
//

#pragma once

namespace BeeEngine
{
    class GameLogger
    {
    public:
        template<typename ...Args>
        static void Info(Args &&...args);
        template<typename ...Args>
        static void Error(Args &&...args);
        template<typename ...Args>
        static void Trace(Args &&...args);
        template<typename ...Args>
        static void Warn(Args &&...args);
    };
}
