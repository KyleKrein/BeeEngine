//
// Created by Александр Лебедев on 06.05.2023.
//
#include "EntryPoint.h"
#include "Debug/Instrumentor.h"
#include "RestartApplication.h"
// #include "SDL_main.h"
#include "Platform/Windows/WindowsUTF8ConsoleOutput.h"
#include "Utils/ShaderConverter.h"

// AllocatorInitializer AllocatorInitializer::instance = AllocatorInitializer();
#include "JobSystem/JobScheduler.h"
namespace BeeEngine
{
    static bool g_Initialized = false;
    static bool g_Restart = false;
    namespace Internal
    {
        void InitEngine()
        {
            BEE_PROFILE_FUNCTION();
            BeeExpects(!g_Initialized);

            Log::Init();
            ShaderConverter::Init();

            g_Initialized = true;
        }

        void ShutDownEngine()
        {
            BEE_PROFILE_FUNCTION();
            BeeExpects(g_Initialized);

            BeeEngine::ShaderConverter::Finalize();

            g_Initialized = false;
        }
    } // namespace Internal
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
    int Main(int argc, char* argv[])
    {
        g_Restart = true;
        while (g_Restart)
        {
            g_Restart = false;
            BEE_DEBUG_START_PROFILING_SESSION("BeeEngineStart", "startup.json");
            Internal::InitEngine();
            BeeEngine::Internal::Job::Initialize();
            Internal::WindowsUTF8ConsoleOutput consoleOutput;
            Application* application = CreateApplication({argc, argv});
            BEE_DEBUG_END_PROFILING_SESSION();
            application->Run();
            BEE_DEBUG_START_PROFILING_SESSION("BeeEngineShutdown", "shutdown.json");
            delete application;
            BeeEngine::Internal::Job::Shutdown();
            Internal::ShutDownEngine();
            BEE_DEBUG_END_PROFILING_SESSION();
        }
        return 0;
    }
#pragma clang diagnostic pop

    void Internal::RestartAfterClosing()
    {
        g_Restart = true;
    }
} // namespace BeeEngine
#if defined(WINDOWS) && !defined(DEBUG)
#pragma comment(linker,                                                                                                \
                "/SUBSYSTEM:WINDOWS"                                                                                   \
                " /ENTRY:mainCRTStartup")
#endif
int main(int argc, char* argv[])
{
    return BeeEngine::Main(argc, argv);
}