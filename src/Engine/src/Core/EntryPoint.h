#pragma once

#include "Application.h"

/*class AllocatorInitializer
{
public:
    AllocatorInitializer()
    {
        GeneralPurposeAllocator::Initialize(nullptr);
    }
private:
    static AllocatorInitializer instance;
};*/

namespace BeeEngine{
    extern Application* CreateApplication();

    static void InitEngine()
    {
        //GeneralPurposeAllocator::Initialize(nullptr);
        Log::Init();
    }
}

extern int main(int argc, char** argv);
