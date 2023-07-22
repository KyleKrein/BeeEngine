//
// Created by alexl on 22.07.2023.
//

#include "GameLogger.h"
#include "Log.h"

namespace BeeEngine
{
    template<typename... Args>
    void GameLogger::Info(Args &&... args)
    {
        BeeInfo(std::forward<Args>(args)...);
    }

    template<typename... Args>
    void GameLogger::Error(Args &&... args)
    {
        BeeError(std::forward<Args>(args)...);
    }

    template<typename... Args>
    void GameLogger::Trace(Args &&... args)
    {
        BeeTrace(std::forward<Args>(args)...);
    }

    template<typename... Args>
    void GameLogger::Warn(Args &&... args)
    {
        BeeWarn(std::forward<Args>(args)...);
    }
}
