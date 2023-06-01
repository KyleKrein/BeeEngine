//
// Created by alexl on 27.05.2023.
//

#pragma once
#include "Core/Logging/Log.h"
#include "version"

#ifdef __cpp_lib_source_location
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
#else
constexpr inline void BeeExpects(bool x, std::string_view file, std::string_view line)
{
    if (x) [[likely]]
        return;
    [[unlikely]]
            ::BeeEngine::Log::GetCoreLogger()->error("Expected result is incorrect at {0}: {1}", file, line);
    debug_break();
}
constexpr inline void BeeEnsures(bool x, std::string_view file, std::string_view line)
{
    if (x) [[likely]]
        return;
    [[unlikely]]
            ::BeeEngine::Log::GetCoreLogger()->error("Expectations failed at {0}: {1}", file, line);
    debug_break();
}
#define BeeExpects(x) BeeExpects(x, __FILE__, std::to_string(__LINE__))
#define BeeEnsures(x) BeeEnsures(x, __FILE__, std::to_string(__LINE__))
#endif