//
// Created by alexl on 26.11.2023.
//
#include <pthread.h>
#include <sched.h>
#include <thread>
#include "Core/Numbers.h"
namespace BeeEngine
{
    void ThreadSetAffinity(std::thread& thread, uint32_t core)
    {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(core, &cpuset);

        pthread_setaffinity_np(thread.native_handle(), sizeof(cpu_set_t), &cpuset);
    }
}