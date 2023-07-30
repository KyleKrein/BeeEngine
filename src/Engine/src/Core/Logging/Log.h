//
// Created by Александр Лебедев on 23.01.2023.
//

#pragma once

#include "spdlog/spdlog.h"
#include "memory.h"
#include "debugbreak.h"
#include <version>
#include "spdlog/sinks/stdout_color_sinks.h"
#if defined(__cpp_lib_source_location)
#include <source_location>
#else
#include "source_location.h"

#endif

namespace BeeEngine{
    class Log
    {
    public:
        static void Init()
        {
            spdlog::set_pattern("%^[%T] %n: %v%$");
            s_CoreLogger = spdlog::stdout_color_mt("BeeEngine");
            s_CoreLogger->set_level(spdlog::level::trace);


            s_ClientLogger = spdlog::stdout_color_mt("App");
            s_ClientLogger->set_level(spdlog::level::trace);

            s_IsInit = true;
        }
        inline constexpr static std::shared_ptr<spdlog::logger>& GetCoreLogger()
        {
            return s_CoreLogger;
        }
        inline static std::shared_ptr<spdlog::logger>& GetClientLogger()
        {
            return s_ClientLogger;
        }

        inline bool static IsInit()
        {
            return s_IsInit;
        }
    private:
        static bool s_IsInit;
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}

//Engine log
/*consteval void BeeLogError(std::string_view message, std::source_location location = std::source_location::current())
{
    std::string final = std::string(message) + " at {1}: {2}";
    ::BeeEngine::Log::GetCoreLogger()->error(final, location.file_name(), location.line());
}*/
#define BeeCoreFatalError(...)  ::BeeEngine::Log::GetCoreLogger()->critical(__VA_ARGS__); throw std::runtime_error(__VA_ARGS__)
#define BeeCoreError(...)  ::BeeEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define BeeCoreWarn(...)   ::BeeEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define BeeCoreInfo(...)   ::BeeEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define BeeCoreTrace(...)  ::BeeEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define BeeCoreAssert(x, ...) if(!(x)) {::BeeEngine::Log::GetCoreLogger()->error(__VA_ARGS__); debug_break();}




//Client log
#define BeeError(...)  ::BeeEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define BeeWarn(...)   ::BeeEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define BeeInfo(...)   ::BeeEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define BeeTrace(...)  ::BeeEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define BeeAssert(x, ...) if(!(x)) ::BeeEngine::Log::GetClientLogger()->error(__VA_ARGS__); debug_break()
