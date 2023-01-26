#pragma once

#include "Application.h"
#include "Core/Logging/Log.h"

namespace BeeEngine{
    extern Application* CreateApplication();

    void InitEngine()
    {
        Log::Init();
    }
}
int main(int argc, char** argv)
{
    BeeEngine::InitEngine();
    BeeEngine::Application* application = BeeEngine::CreateApplication();
    application->Run();
    delete application;
    return 0;
}
