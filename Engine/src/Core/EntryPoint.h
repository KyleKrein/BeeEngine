#pragma once

#include "Application.h"

namespace BeeEngine{
    extern Application* CreateApplication();
}
int main(int argc, char** argv)
{
    BeeEngine::Application* application = BeeEngine::CreateApplication();
    application->Run();
    delete application;
    return 0;
}
