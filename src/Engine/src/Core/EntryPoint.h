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
    extern gsl::not_null<Application*> CreateApplication();

    static void InitEngine()
    {
        BEE_PROFILE_FUNCTION();
        static bool initialized = false;
        if(initialized)
        {
            BeeCoreError("Engine was already initialized");
        }
        Log::Init();

        initialized = true;
    }
}

extern int main(int argc, char** argv);

