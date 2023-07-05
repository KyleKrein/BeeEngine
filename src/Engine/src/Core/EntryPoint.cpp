//
// Created by Александр Лебедев on 06.05.2023.
//
#include "EntryPoint.h"
#include "Debug/Instrumentor.h"
#include "SDL_main.h"
#include "Utils/ShaderConverter.h"

//AllocatorInitializer AllocatorInitializer::instance = AllocatorInitializer();

namespace BeeEngine
{
    void InitEngine()
    {
        BEE_PROFILE_FUNCTION();
        static bool initialized = false;
        if(initialized)
        {
            BeeCoreError("Engine was already initialized");
            return;
        }
        Log::Init();

        ShaderConverter::Init();

        initialized = true;
    }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
int main(int argc, char *argv[])
{
    BEE_DEBUG_START_PROFILING_SESSION("BeeEngineStart", "startup.json");
    BeeEngine::InitEngine();
    BeeEngine::Application* application = BeeEngine::CreateApplication();
    BEE_DEBUG_END_PROFILING_SESSION();
    application->Run();
    BEE_DEBUG_START_PROFILING_SESSION("BeeEngineShutdown", "shutdown.json");
    delete application;
    BeeEngine::ShaderConverter::Finalize();
    BEE_DEBUG_END_PROFILING_SESSION();
    return 0;
}
#pragma clang diagnostic pop
