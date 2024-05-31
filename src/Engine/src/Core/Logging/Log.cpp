//
// Created by alexl on 26.01.2023.
//
#include "Log.h"
#include "Debug/Instrumentor.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace BeeEngine
{
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
    bool Log::s_IsInit = false;
} // namespace BeeEngine
