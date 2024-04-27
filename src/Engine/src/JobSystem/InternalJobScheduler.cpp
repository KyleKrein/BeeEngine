//
// Created by Aleksandr on 11.03.2024.
//
#include "InternalJobScheduler.h"

//IMPORTANT: DO NOT TOUCH THIS FILE
//This code needs to be in this file to awoid bugs
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
}