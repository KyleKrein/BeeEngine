//
// Created by alexl on 22.07.2023.
//

#pragma once
#include <string_view>
namespace BeeEngine
{
    class GameLogger
    {
    public:
        template<typename ...Args>
        static void Info(std::string_view format, Args &&...args);
        template<typename ...Args>
        static void Error(std::string_view format, Args &&...args);
        template<typename ...Args>
        static void Trace(std::string_view format, Args &&...args);
        template<typename ...Args>
        static void Warn(std::string_view format, Args &&...args);
    };
}
