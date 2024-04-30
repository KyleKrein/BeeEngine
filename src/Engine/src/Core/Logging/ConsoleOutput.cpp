//
// Created by alexl on 14.10.2023.
//

#include "ConsoleOutput.h"
#include "Core/Logging/Log.h"

namespace BeeEngine
{
    IConsoleOutputProvider* ConsoleOutput::s_OutputProvider = nullptr;
    void ConsoleOutput::Log(const String &message, ConsoleOutput::Level level, Input input)
    {
        if(s_OutputProvider)
            s_OutputProvider->AddMessage(message,level, input);
        LogToNativeConsole(message,level, input);
    }

    void ConsoleOutput::LogToNativeConsole(const String &message, ConsoleOutput::Level level
            , ConsoleOutput::Input input)
    {
        std::shared_ptr<spdlog::logger>& logger = input == Input::Engine ? Log::GetCoreLogger() : Log::GetClientLogger();
        switch (level)
        {
            case Level::Error:
                logger->error(message);
                break;
            case Level::Warning:
                logger->warn(message);
                break;
            case Level::Information:
                logger->info(message);
                break;
            case Level::Trace:
                logger->trace(message);
                break;
        }
    }

    void ConsoleOutput::SetOutputProvider(IConsoleOutputProvider *provider)
    {
        s_OutputProvider = provider;
    }
    IConsoleOutputProvider *ConsoleOutput::GetOutputProvider()
    {
        return s_OutputProvider;
    }
}