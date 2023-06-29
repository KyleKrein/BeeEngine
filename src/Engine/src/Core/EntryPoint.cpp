//
// Created by Александр Лебедев on 06.05.2023.
//
#include "EntryPoint.h"
#include "Debug/Instrumentor.h"
#include "SDL_main.h"

//AllocatorInitializer AllocatorInitializer::instance = AllocatorInitializer();


int main(int argc, char *argv[])
{
    BEE_DEBUG_START_PROFILING_SESSION("BeeEngineStart", "startup.json");
    BeeEngine::InitEngine();
    BeeEngine::Application* application = BeeEngine::CreateApplication();
    BEE_DEBUG_END_PROFILING_SESSION();
    application->Run();
    BEE_DEBUG_START_PROFILING_SESSION("BeeEngineShutdown", "shutdown.json");
    delete application;
    BEE_DEBUG_END_PROFILING_SESSION();
    return 0;
}
