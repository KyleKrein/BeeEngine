//
// Created by alexl on 27.11.2023.
//
#include "JobScheduler.h"
namespace BeeEngine
{
    Internal::JobScheduler *Job::s_Instance = nullptr;

    namespace Internal
    {
        thread_local Ref<Internal::Fiber> JobScheduler::s_CurrentFiber = nullptr;
        thread_local boost::context::continuation JobScheduler::s_MainContext{};

        Ref<Internal::Fiber> &JobScheduler::GetCurrentFiber()
        {
            return s_CurrentFiber;
        }

        boost::context::continuation &JobScheduler::GetMainContext()
        {
            return s_MainContext;
        }
    } // Internal
} // BeeEngine