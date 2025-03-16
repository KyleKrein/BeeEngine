//
// Created by Александр Лебедев on 23.01.2023.
//

#pragma once

#include "Core/Format.h"
#include "Core/Logging/ConsoleOutput.h"
#include "Core/ToString.h"
#include "debugbreak.h"
#include "memory.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <filesystem>
#include <iterator>
#include <memory>
#include <spdlog/sinks/daily_file_sink.h>
#include <system_error>
#include <vector>
#include <version>

#if defined(__cpp_lib_source_location)
#include <source_location>
#else
#include "source_location.h"

#endif

namespace BeeEngine
{
    class Log
    {
    public:
        static void Init()
        {
            std::error_code err;
#if defined(LINUX)
            const char* logfileName = "/tmp/beeengine/log.txt";
            std::filesystem::create_directory("/tmp/beeengine", err);            
#elif defined(WINDOWS)
const char* logfileName = "log.txt";
#elif defined(MACOS)
const char* logfileName = "/tmp/beeengine/log.txt";
  std::filesystem::create_directory("/tmp/beeengine", err);
#else
const char* logfileName = "log.txt";
#endif
std::filesystem::remove_all(logfileName, err);
            std::string pattern = "%^[%T] [%l] %n: %v%$";
            spdlog::set_pattern(pattern);
            std::vector<spdlog::sink_ptr> sinks{
                std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
                std::make_shared<spdlog::sinks::daily_file_format_sink_mt>(logfileName, 23, 59)};

            s_CoreLogger = std::make_shared<spdlog::logger>("BeeEngine", std::begin(sinks), std::end(sinks));
            s_CoreLogger->set_level(spdlog::level::trace);
            s_CoreLogger->set_pattern(pattern);

            s_ClientLogger = std::make_shared<spdlog::logger>("App", std::begin(sinks), std::end(sinks));
            s_ClientLogger->set_level(spdlog::level::trace);
            s_ClientLogger->set_pattern(pattern);

            s_IsInit = true;
        }
        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

        inline bool static IsInit() { return s_IsInit; }

    private:
        static bool s_IsInit;
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
} // namespace BeeEngine

// Engine log
/*consteval void BeeLogError(std::string_view message, std::source_location location = std::source_location::current())
{
    std::string final = std::string(message) + " at {1}: {2}";
    ::BeeEngine::Log::GetCoreLogger()->error(final, location.file_name(), location.line());
}*/
#define BeeCoreFatalError(...)                                                                                         \
    ::BeeEngine::ConsoleOutput::Log(::BeeEngine::FormatString("[Thread {0}] ", std::this_thread::get_id()) +           \
                                        ::BeeEngine::FormatString(__VA_ARGS__),                                        \
                                    ::BeeEngine::ConsoleOutput::Level::Error,                                          \
                                    ::BeeEngine::ConsoleOutput::Input::Engine);                                        \
    throw std::runtime_error(__VA_ARGS__)
#define BeeCoreError(...)                                                                                              \
    ::BeeEngine::ConsoleOutput::Log(::BeeEngine::FormatString("[Thread {0}] ", std::this_thread::get_id()) +           \
                                        ::BeeEngine::FormatString(__VA_ARGS__),                                        \
                                    ::BeeEngine::ConsoleOutput::Level::Error,                                          \
                                    ::BeeEngine::ConsoleOutput::Input::Engine)
#define BeeCoreWarn(...)                                                                                               \
    ::BeeEngine::ConsoleOutput::Log(::BeeEngine::FormatString("[Thread {0}] ", std::this_thread::get_id()) +           \
                                        ::BeeEngine::FormatString(__VA_ARGS__),                                        \
                                    ::BeeEngine::ConsoleOutput::Level::Warning,                                        \
                                    ::BeeEngine::ConsoleOutput::Input::Engine)
#define BeeCoreInfo(...)                                                                                               \
    ::BeeEngine::ConsoleOutput::Log(::BeeEngine::FormatString("[Thread {0}] ", std::this_thread::get_id()) +           \
                                        ::BeeEngine::FormatString(__VA_ARGS__),                                        \
                                    ::BeeEngine::ConsoleOutput::Level::Information,                                    \
                                    ::BeeEngine::ConsoleOutput::Input::Engine)
#if defined(Debug)
#define BeeCoreTrace(...)                                                                                              \
    ::BeeEngine::ConsoleOutput::Log(::BeeEngine::FormatString("[Thread {0}] ", std::this_thread::get_id()) +           \
                                        ::BeeEngine::FormatString(__VA_ARGS__),                                        \
                                    ::BeeEngine::ConsoleOutput::Level::Trace,                                          \
                                    ::BeeEngine::ConsoleOutput::Input::Engine)
#else
#define BeeCoreTrace(...)
#endif
#define BeeCoreAssert(x, ...)                                                                                          \
    if (!(x))                                                                                                          \
    {                                                                                                                  \
        ::BeeEngine::ConsoleOutput::Log(::BeeEngine::FormatString("[Thread {0}] ", std::this_thread::get_id()) +       \
                                            ::BeeEngine::FormatString(__VA_ARGS__),                                    \
                                        ::BeeEngine::ConsoleOutput::Level::Error,                                      \
                                        ::BeeEngine::ConsoleOutput::Input::Engine);                                    \
        debug_break();                                                                                                 \
    }                                                                                                                  \
/*#define BeeCoreFatalError(...)  ::BeeEngine::Log::GetCoreLogger()->critical(::BeeEngine::FormatString(__VA_ARGS__)); \
throw std::runtime_error(__VA_ARGS__) #define BeeCoreError(...)                                                        \
::BeeEngine::Log::GetCoreLogger()->error(::BeeEngine::FormatString(__VA_ARGS__)) #define BeeCoreWarn(...)              \
::BeeEngine::Log::GetCoreLogger()->warn(::BeeEngine::FormatString(__VA_ARGS__)) #define BeeCoreInfo(...)               \
::BeeEngine::Log::GetCoreLogger()->info(::BeeEngine::FormatString(__VA_ARGS__)) #define BeeCoreTrace(...)              \
::BeeEngine::Log::GetCoreLogger()->trace(::BeeEngine::FormatString(__VA_ARGS__)) #define BeeCoreAssert(x, ...)         \
if(!(x)) {::BeeEngine::Log::GetCoreLogger()->error(::BeeEngine::FormatString(__VA_ARGS__)); debug_break();}*/

// Client log
#define BeeError(...)                                                                                                  \
    ::BeeEngine::ConsoleOutput::Log(::BeeEngine::FormatString("[Thread {0}] ", std::this_thread::get_id()) +           \
                                        ::BeeEngine::FormatString(__VA_ARGS__),                                        \
                                    ::BeeEngine::ConsoleOutput::Level::Error,                                          \
                                    ::BeeEngine::ConsoleOutput::Input::App)
#define BeeWarn(...)                                                                                                   \
    ::BeeEngine::ConsoleOutput::Log(::BeeEngine::FormatString("[Thread {0}] ", std::this_thread::get_id()) +           \
                                        ::BeeEngine::FormatString(__VA_ARGS__),                                        \
                                    ::BeeEngine::ConsoleOutput::Level::Warning,                                        \
                                    ::BeeEngine::ConsoleOutput::Input::App)
#define BeeInfo(...)                                                                                                   \
    ::BeeEngine::ConsoleOutput::Log(::BeeEngine::FormatString("[Thread {0}] ", std::this_thread::get_id()) +           \
                                        ::BeeEngine::FormatString(__VA_ARGS__),                                        \
                                    ::BeeEngine::ConsoleOutput::Level::Information,                                    \
                                    ::BeeEngine::ConsoleOutput::Input::App)
#define BeeTrace(...)                                                                                                  \
    ::BeeEngine::ConsoleOutput::Log(::BeeEngine::FormatString("[Thread {0}] ", std::this_thread::get_id()) +           \
                                        ::BeeEngine::FormatString(__VA_ARGS__),                                        \
                                    ::BeeEngine::ConsoleOutput::Level::Trace,                                          \
                                    ::BeeEngine::ConsoleOutput::Input::App)
// #define BeeAssert(x, ...) if(!(x)) ::BeeEngine::Log::GetClientLogger()->error(__VA_ARGS__); debug_break()
