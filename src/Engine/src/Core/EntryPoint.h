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
    class ApplicationArgs
    {
    public:
        const std::vector<UTF8String>& GetArguments() const
        {
            return m_Args;
        }
        [[deprecated("Use GetArgs() instead")]]
        int GetArgc() const
        {
            return argc;
        }
        [[deprecated("Use GetArgs() instead")]]
        char** GetArgv() const
        {
            return argv;
        }

        ApplicationArgs(int argc, char** argv)
        : argc(argc), argv(argv)
        {
            for (int i = 0; i < argc; ++i)
            {
                m_Args.emplace_back(argv[i]);
                BeeExpects(IsValidString(m_Args.back()));
            }
        }
    private:
        int argc;
        char** argv;
        std::vector<UTF8String> m_Args;
    };
    extern gsl::not_null<Application*> CreateApplication(const ApplicationArgs& args);
}

#ifdef __cplusplus
extern "C" int main(int argc, char* argv[]);
#endif

