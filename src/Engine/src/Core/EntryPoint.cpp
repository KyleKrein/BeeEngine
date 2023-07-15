//
// Created by Александр Лебедев on 06.05.2023.
//
#include "EntryPoint.h"
#include "RestartApplication.h"
#include "Debug/Instrumentor.h"
#include "SDL_main.h"
#include "Utils/ShaderConverter.h"

//AllocatorInitializer AllocatorInitializer::instance = AllocatorInitializer();

namespace BeeEngine
{
    static bool g_Initialized = false;
    static bool g_Restart = false;
    static void InitEngine()
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
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
    static int Main(int argc, char *argv[])
    {
        g_Restart = true;
        while (g_Restart)
        {
            g_Restart = false;
            BEE_DEBUG_START_PROFILING_SESSION("BeeEngineStart", "startup.json");
            InitEngine();
            Application* application = CreateApplication();
            BEE_DEBUG_END_PROFILING_SESSION();
            application->Run();
            BEE_DEBUG_START_PROFILING_SESSION("BeeEngineShutdown", "shutdown.json");
            delete application;
            ShutDownEngine();
            BEE_DEBUG_END_PROFILING_SESSION();
        }
        return 0;
    }
#pragma clang diagnostic pop

    void Internal::RestartAfterClosing()
    {
        g_Restart = true;
    }
}

int main(int argc, char *argv[])
{
    return BeeEngine::Main(argc, argv);
}
