//
// Created by alexl on 27.05.2023.
//

#pragma once
#include "Core/Logging/Log.h"
#include "source_location"

constexpr inline void BeeExpects(bool x, std::source_location location = std::source_location::current())
{
    if (x) [[likely]]
        return;
    [[unlikely]]
    ::BeeEngine::Log::GetCoreLogger()->error("Expected result is incorrect at {0}: {1}", location.file_name(), location.line());
    debug_break();
}
constexpr inline void BeeEnsures(bool x, std::source_location location = std::source_location::current())
{
    if (x) [[likely]]
        return;
    [[unlikely]]
    ::BeeEngine::Log::GetCoreLogger()->error("Expectations failed at {0}: {1}", location.file_name(), location.line());
    debug_break();
}