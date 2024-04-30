//
// Created by alexl on 14.10.2023.
//

#pragma once
#include "Core/String.h"

namespace BeeEngine
{
    class IConsoleOutputProvider;
    class ConsoleOutput
    {
    public:
        enum class Level{Error,Warning,Information,Trace};
        enum class Input {Engine, App};
        static void Log(const String& message, Level level, Input input = Input::App);
        static void SetOutputProvider(IConsoleOutputProvider* provider);
        static IConsoleOutputProvider* GetOutputProvider();
    private:
        static void LogToNativeConsole(const String &message, ConsoleOutput::Level level
                , ConsoleOutput::Input input);
        static IConsoleOutputProvider* s_OutputProvider;
    };
    class IConsoleOutputProvider
    {
    public:
        virtual void AddMessage(const String& message, ConsoleOutput::Level level, ConsoleOutput::Input input) = 0;
    };
}