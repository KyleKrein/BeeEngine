//
// Created by alexl on 14.10.2023.
//

#pragma once
#include <vector>
#include "Core/String.h"
#include "Core/Logging/ConsoleOutput.h"

namespace BeeEngine
{
    class ImGuiOutputConsole: public IConsoleOutputProvider
    {
    public:
        void AddMessage(const String& message, ConsoleOutput::Level level, ConsoleOutput::Input input) override
        {
            Messages.emplace_back(message,level, input);
        }
        void RenderGUI();
        void Toggle()
        {
            m_IsOpen = !m_IsOpen;
        }
    private:
        struct Message
        {
            String Text;
            ConsoleOutput::Level Level;
            ConsoleOutput::Input Input;
            Message(const String& message, ConsoleOutput::Level level, ConsoleOutput::Input input)
            : Text(message), Level(level), Input(input) {}
        };
        std::vector <Message> Messages;
        bool m_IsOpen = true;
        bool m_ShowErrors = true;
        bool m_ShowWarnings = true;
        bool m_ShowInformation = true;
        bool m_ShowTrace = false;
    };
}
