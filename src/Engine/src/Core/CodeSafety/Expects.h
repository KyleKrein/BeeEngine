//
// Created by alexl on 27.05.2023.
//

#pragma once
#include "Core/Logging/Log.h"
#include "version"

#include "StackTrace.h"

inline void BeeExpects(const char* expr, BeeEngine::StackTrace&& stackTrace = BeeEngine::StackTrace())
{
    BeeCoreError("Expected result {0} is incorrect\nStacktrace:\n{1}", expr, stackTrace);
    debug_break();
}
inline void BeeEnsures(const char* expr, BeeEngine::StackTrace&& stackTrace = BeeEngine::StackTrace())
{
    BeeCoreError("Expectations {0} failed\nStacktrace:\n{1}", expr, stackTrace);
    debug_break();
}

#if defined(BEE_ENABLE_CHECKS)
#define BEE_STRINGIFY(x) #x
    #define BeeExpects(x) if(!(x)) [[unlikely]] BeeExpects(BEE_STRINGIFY(x))
    #define BeeEnsures(x) if(!(x)) [[unlikely]] BeeEnsures(BEE_STRINGIFY(x))
//#undef STRINGIFY
#else
    #define BeeExpects(x)
    #define BeeEnsures(x)
#endif
#if 0
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