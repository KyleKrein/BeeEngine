//
// Created by alexl on 08.06.2023.
//

#pragma once

#include <string>

namespace BeeEngine
{
    class DebugLog
    {
    public:
        template <typename... Args>
        static void Log(std::string_view format, Args... args);
        template <typename... Args>
        static void Error(std::string_view format, Args... args);
        template <typename... Args>
        static void Warning(std::string_view format, Args... args);
    };
} // namespace BeeEngine
