//
// Created by Александр Лебедев on 06.05.2023.
//
#include "EntryPoint.h"

int main(int argc, char** argv)
{
    BeeEngine::InitEngine();
    BeeEngine::Application* application = BeeEngine::CreateApplication();
    application->Run();
    delete application;
    print_unfreed_mem();
    return 0;
}