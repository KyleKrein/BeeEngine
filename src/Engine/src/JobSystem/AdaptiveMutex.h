//
// Created by alexl on 26.11.2023.
//

#pragma once
#include "JobScheduler.h"
#include <atomic>
#include <chrono>
namespace BeeEngine::Jobs
{
    /**
     * @brief Implements an adaptive mutex with a timeout mechanism.
     *
     * This class provides a lock mechanism that allows multiple jobs to access a shared resource,
     * but with an added timeout feature. If a job fails to acquire the lock within the specified
     * timeout period, it yields the thread to other jobs.
     */
    class AdaptiveMutex
    {
    public:
        /**
         * @brief Attempts to acquire the lock within the specified timeout period.
         *
         * If the lock is already held by another job, this function will wait until the lock becomes
         * available or the timeout period is exceeded. If the timeout period is exceeded, the function
         * yields the thread to other jobs and retries acquiring the lock.
         *
         * @param timeout The maximum time to wait for the lock, in milliseconds. The default value is 1000
         * milliseconds.
         */
        void lock(std::chrono::milliseconds timeout = std::chrono::milliseconds(1000))
        {
        StartOfLock:
            auto start = std::chrono::high_resolution_clock::now();
            while (flag.test_and_set(std::memory_order_acquire))
            {
                // Active waiting (spinning)
                auto end = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                if (elapsed > timeout)
                {
                    Jobs::this_job::yield();
                    goto StartOfLock;
                }
            }
        }

        /**
         * @brief Releases the lock.
         *
         * This function releases the lock, allowing other jobs to acquire it. It should be called
         * only by the job that currently holds the lock.
         * Otherwise the behavior is undefined.
         */
        void unlock() { flag.clear(std::memory_order_release); }

        /**
         * @brief Attempts to acquire the lock without blocking.
         *
         * This function attempts to acquire the lock without blocking. If the lock is already held by
         * another job, it returns false immediately. Otherwise, it acquires the lock and returns true.
         *
         * @return true if the lock was acquired successfully, false otherwise.
         */
        [[nodiscard]] bool try_lock() { return !flag.test_and_set(std::memory_order_acquire); }

    private:
        std::atomic_flag flag = {};
    };
} // namespace BeeEngine::Jobs