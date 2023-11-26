//
// Created by alexl on 26.11.2023.
//
#include <mach/thread_policy.h>
#include <mach/thread_act.h>
#include <pthread.h>
#include <thread>
#include "Core/Numbers.h"
namespace BeeEngine::Internal
{
    void ThreadSetAffinity(std::thread& thread, uint32_t core)
    {
        thread_affinity_policy_data_t policy = { core };
        thread_port_t machThread = pthread_mach_thread_np(thread.native_handle());
        thread_policy_set(machThread, THREAD_AFFINITY_POLICY, (thread_policy_t)&policy, 1);
    }
}