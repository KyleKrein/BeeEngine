#pragma once
#include <BeeEngine.h>
#include <FileSystem/File.h>
#include <Gui/ImGui/FpsCounter.h>
#include <Gui/ImGui/ImGuiOutputConsole.h>
#include <Gui/ImGui/RendererStatisticsGUI.h>

namespace BeeEngine::Runtime
{
    class DebugLayer : public Layer
    {
    public:
        DebugLayer()
        {
            ConsoleOutput::SetOutputProvider(&m_Console);
            /*
            std::set_terminate([]()
            {
                std::exception_ptr p = std::current_exception();
                if (p)
                {
                    try
                    {
                        std::rethrow_exception(p);
                    }
                    catch (const std::exception& e)
                    {
                        BeeCoreError("Unhandled Exception. Exception: {0}", e.what());
                    }
                    catch (...)
                    {
                        BeeCoreError("Unhandled Exception. Unknown exception.");
                    }
                }
                StackTrace st;
                BeeCoreError("std::terminate() called. Stack trace:\n{0}", st);
                if(auto* consoleProvider = static_cast<ImGuiOutputConsole*>(ConsoleOutput::GetOutputProvider()))
                {
                    String dump = consoleProvider->GetDump();
                    File::WriteFile("crash_dump.txt", dump);
                }
                std::abort();
            });*/
        }
        void OnUpdate(FrameData& frameData) override
        {
            BeeCoreTrace("{}", std::source_location::current().function_name());
            m_FpsCounter.Update();
        }
        void OnGUIRendering() override
        {
            BeeCoreTrace("{}", std::source_location::current().function_name());
            m_FpsCounter.Render();
            m_RendererStatisticsGUI.Render();
            m_Console.RenderGUI();
            BeeCoreTrace("Finished {}", std::source_location::current().function_name());
        }
        ~DebugLayer() noexcept override { ConsoleOutput::SetOutputProvider(nullptr); }
        BeeEngine::ImGuiOutputConsole& GetConsole() { return m_Console; }

    private:
        BeeEngine::Internal::FpsCounter m_FpsCounter{};
        BeeEngine::Internal::RendererStatisticsGUI m_RendererStatisticsGUI{};
        BeeEngine::ImGuiOutputConsole m_Console{};
    };
} // namespace BeeEngine::Runtime