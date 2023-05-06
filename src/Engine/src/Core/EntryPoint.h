#pragma once

#include "Application.h"

namespace BeeEngine{
    extern Application* CreateApplication();

    static void InitEngine()
    {
        Log::Init();
    }
}

extern int main(int argc, char** argv);
