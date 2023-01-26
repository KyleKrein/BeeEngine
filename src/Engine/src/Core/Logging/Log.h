//
// Created by Александр Лебедев on 23.01.2023.
//

#pragma once

#include "spdlog/spdlog.h"
#include "memory.h"
#include "debugbreak.h"

namespace BeeEngine{
    class Log
    {
    public:
        static void Init();
        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger()
        {
            return s_CoreLogger;
        }
        inline static std::shared_ptr<spdlog::logger>& GetClientLogger()
        {
            return s_ClientLogger;
        }
    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}
//Engine log
#define BeeCoreError(...)  ::BeeEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define BeeCoreWarn(...)   ::BeeEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define BeeCoreInfo(...)   ::BeeEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define BeeCoreTrace(...)  ::BeeEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define BeeCoreAssert(x, ...) if(!x) ::BeeEngine::Log::GetCoreLogger()->error(__VA_ARGS__); debug_break()



//Client log
#define BeeError(...)  ::BeeEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define BeeWarn(...)   ::BeeEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define BeeInfo(...)   ::BeeEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define BeeTrace(...)  ::BeeEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define BeeAssert(x, ...) if(!x) ::BeeEngine::Log::GetClientLogger()->error(__VA_ARGS__); debug_break()
