//
// Created by alexl on 27.11.2023.
//

#pragma once
#include <atomic>
#include "JobScheduler.h"
namespace BeeEngine::Jobs
{
    class Mutex
    {
    public:
        void lock()
        {
            while (flag.test_and_set(std::memory_order_acquire))
            {
                this_job::yield();
            }
        }
        void unlock() {
            flag.clear(std::memory_order_release);
        }

    private:
        std::atomic_flag flag = {};
    };
}