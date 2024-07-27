/**
 * @file InternalJobScheduler.cpp
 * @author Aleksandr Lebedev
 * @brief This file contains definitions of thread_local static fields
 * and functions to get values from those fields. They are separated
 * from all other files to avoid certain compiler optimizations,
 * that assume that all thread_local fields always have the same
 * memory address, which may not be the case, because this JobSystem
 * can move Jobs between threads.
 * @date 2024-07-27
 *
 * @copyright Copyright (c) 2024
 *
 */
//
// Created by Aleksandr on 11.03.2024.
//
#include "InternalJobScheduler.h"

// IMPORTANT: DO NOT TOUCH THIS FILE
// This code needs to be in this file to awoid bugs
namespace BeeEngine
{
    Internal::JobScheduler* Internal::Job::s_Instance = nullptr;

    namespace Internal
    {
        thread_local Ref<Internal::Fiber> JobScheduler::s_CurrentFiber = nullptr;
        thread_local boost::context::continuation JobScheduler::s_MainContext{};

        Ref<Internal::Fiber>& JobScheduler::GetCurrentFiber()
        {
            return s_CurrentFiber;
        }

        boost::context::continuation& JobScheduler::GetMainContext()
        {
            return s_MainContext;
        }
    } // namespace Internal
} // namespace BeeEngine