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
}

#ifdef __cplusplus
extern "C" int main(int argc, char* argv[]);
#endif

