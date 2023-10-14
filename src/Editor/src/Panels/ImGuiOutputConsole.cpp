//
// Created by alexl on 14.10.2023.
//

#include "ImGuiOutputConsole.h"
#include "imgui.h"
#include "Core/Color4.h"
#include <Core/Logging/Log.h>
#include <misc/cpp/imgui_stdlib.h>
#include <ctime>


namespace BeeEngine
{

    Color4 Level2Color(ConsoleOutput::Level level)
    {
        switch (level)
        {
            case ConsoleOutput::Level::Error:
                return  Color4::Red;
            case ConsoleOutput::Level::Warning:
                return  Color4::Yellow;
            case ConsoleOutput::Level::Information:
                return  Color4::Green;
            case ConsoleOutput::Level::Trace:
                return Color4::Gray;
        }

    }
    void ImGuiOutputConsole::RenderGUI()
    {
        if(!m_IsOpen)
            return;
        ImGui::Begin("Output Console", &m_IsOpen);
        ImGui::Checkbox("Show Errors",&m_ShowErrors);
        ImGui::SameLine();
        ImGui::Checkbox("Show Warnings",&m_ShowWarnings);
        ImGui::SameLine();
        ImGui::Checkbox("Show Information",&m_ShowInformation);
        ImGui::SameLine();
        ImGui::Checkbox("Show Trace",&m_ShowTrace);
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            for (auto &message: m_Messages)
            {
                if (message.Level == ConsoleOutput::Level::Error and not m_ShowErrors)
                    continue;
                if (message.Level == ConsoleOutput::Level::Warning and not m_ShowWarnings)
                    continue;
                if (message.Level == ConsoleOutput::Level::Information and not m_ShowInformation)
                    continue;
                if (message.Level == ConsoleOutput::Level::Trace and not m_ShowTrace)
                    continue;
                ImGui::TextColored(Level2Color(message.Level), message.TimeFormatted.c_str());
                ImGui::SameLine();
                ImGui::TextColored(Level2Color(message.Level),
                                   message.Input == ConsoleOutput::Input::Engine ? "Engine: " : "App: ");
                ImGui::SameLine();
                ImGui::TextColored(Level2Color(message.Level), message.Text.c_str());
            }
        }

        ImGui::End();
    }

    std::string ImGuiOutputConsole::GetCurrentTimeFormatted()
    {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
        struct std::tm* timeInfo = std::localtime(&currentTime);

        std::array<char,11> buffer{};  // "[hh:mm:ss]" + null-terminator
        std::strftime(buffer.data(), buffer.size(), "[%T]", timeInfo);

        return {buffer.data()};
    }
}
