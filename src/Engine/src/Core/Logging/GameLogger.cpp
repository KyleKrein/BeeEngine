//
// Created by alexl on 22.07.2023.
//

#include "GameLogger.h"
#include "Log.h"

namespace BeeEngine
{
    template<typename... Args>
    void GameLogger::Info(std::string_view format, Args &&... args)
    {
        BeeInfo(format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void GameLogger::Error(std::string_view format, Args &&... args)
    {
        BeeError(format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void GameLogger::Trace(std::string_view format, Args &&... args)
    {
        BeeTrace(format, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void GameLogger::Warn(std::string_view format, Args &&... args)
    {
        BeeWarn(format, std::forward<Args>(args)...);
    }
}
