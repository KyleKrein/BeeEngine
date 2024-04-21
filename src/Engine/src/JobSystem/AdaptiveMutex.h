//
// Created by alexl on 26.11.2023.
//

#pragma once
#include <atomic>
#include <chrono>
#include "JobScheduler.h"
namespace BeeEngine::Jobs
{
    class AdaptiveMutex
    {
    public:
        void lock(std::chrono::milliseconds timeout = std::chrono::milliseconds(1000))
        {
            StartOfLock:
            auto start = std::chrono::high_resolution_clock::now();
            while (flag.test_and_set(std::memory_order_acquire)) {
                // Активное ожидание (вращение)
                auto end = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                if (elapsed > timeout) {
                    Jobs::this_job::yield();
                    goto StartOfLock;
                }
            }
        }

        void unlock()
        {
            flag.clear(std::memory_order_release);
        }

        [[nodiscard]]bool try_lock()
        {
            return !flag.test_and_set(std::memory_order_acquire);
        }

    private:
        std::atomic_flag flag = {};
    };
}