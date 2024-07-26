//
// Created by alexl on 27.11.2023.
//

#pragma once
#include "JobScheduler.h"
#include <atomic>
namespace BeeEngine::Jobs
{
    /**
     * @brief A class representing a mutex for synchronization in a jobs environment.
     *
     * This class provides methods for acquiring and releasing ownership of the mutex.
     * It uses atomic operations to ensure thread safety.
     */
    class Mutex
    {
    public:
        /**
         * @brief Acquires ownership of the mutex.
         *
         * This tries to acquire ownership of the mutex. If the mutex
         * is already owned, this function yields the current job.
         * Meantime other jobs will be scheduled to run.
         */
        void lock()
        {
            while (flag.test_and_set(std::memory_order_acquire))
            {
                this_job::yield();
            }
        }

        /**
         * @brief Releases ownership of the mutex.
         *
         * This function releases ownership of the mutex, allowing other jobs to acquire it.
         * It is assumed that the calling job currently owns the mutex.
         * If not, behavior is undefined
         */
        void unlock() { flag.clear(std::memory_order_release); }

        /**
         * @brief Attempts to acquire ownership of the mutex without blocking.
         *
         * This function attempts to acquire ownership of the mutex without blocking.
         * If the mutex is already owned by another job, the function returns false.
         * If the mutex is successfully acquired, the function returns true.
         *
         * @return true if the mutex was successfully acquired, false otherwise.
         */
        [[nodiscard]] bool try_lock() { return !flag.test_and_set(std::memory_order_acquire); }

    private:
        std::atomic_flag flag = {};
    };
} // namespace BeeEngine::Jobs