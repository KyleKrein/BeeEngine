//
// Created by alexl on 14.10.2023.
//

#pragma once
#include "Core/Logging/ConsoleOutput.h"
#include "Core/String.h"
#include <mutex>
#include <vector>

namespace BeeEngine
{
    class ImGuiOutputConsole : public IConsoleOutputProvider
    {
    public:
        void AddMessage(const String& message, ConsoleOutput::Level level, ConsoleOutput::Input input) override
        {
            auto now = GetCurrentTimeFormatted();
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Messages.emplace_back(message, level, input, std::move(now));
        }
        void RenderGUI();
        void Toggle() { m_IsOpen = !m_IsOpen; }
        void Clean()
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Messages.clear();
        }
        String GetDump();
        bool IsOpen() const { return m_IsOpen; }

    private:
        std::string GetCurrentTimeFormatted();
        struct Message
        {
            String Text;
            ConsoleOutput::Level Level;
            ConsoleOutput::Input Input;
            String TimeFormatted;
            Message(const String& message, ConsoleOutput::Level level, ConsoleOutput::Input input, String&& time)
                : Text(message), Level(level), Input(input), TimeFormatted(std::move(time))
            {
            }
        };
        std::vector<Message> m_Messages;
        std::mutex m_Mutex;
        bool m_IsOpen = true;
        bool m_ShowErrors = true;
        bool m_ShowWarnings = true;
        bool m_ShowInformation = true;
        bool m_ShowTrace = false;
    };
} // namespace BeeEngine
