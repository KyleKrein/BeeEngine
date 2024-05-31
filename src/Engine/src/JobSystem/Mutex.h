//
// Created by alexl on 27.11.2023.
//

#pragma once
#include "JobScheduler.h"
#include <atomic>
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
        void unlock() { flag.clear(std::memory_order_release); }

        [[nodiscard]] bool try_lock() { return !flag.test_and_set(std::memory_order_acquire); }

    private:
        std::atomic_flag flag = {};
    };
} // namespace BeeEngine::Jobs