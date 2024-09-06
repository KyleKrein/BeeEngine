//
// Created by alexl on 26.11.2023.
//

#pragma once
#include <atomic>
namespace BeeEngine::Jobs
{
    /**
     * @brief A simple spinlock implementation using std::atomic_flag.
     *
     * This class provides a basic spinlock mechanism using atomic operations.
     * The lock() function acquires the lock by spinning until the flag is cleared.
     * The unlock() function releases the lock by clearing the flag.
     * The try_lock() function attempts to acquire the lock without spinning.
     * The is_locked() function checks if the lock is currently held.
     */
    class SpinLock
    {
    public:
        /**
         * @brief Acquires the spinlock.
         *
         * Spins until the lock is acquired by setting the atomic flag.
         * This function will block the calling thread until the lock is available.
         */
        void lock()
        {
            while (flag.test_and_set(std::memory_order_acquire))
            {
                // Active waiting (spinning)
            }
        }

        /**
         * @brief Releases the spinlock.
         *
         * Clears the atomic flag, allowing other threads to acquire the lock.
         */
        void unlock() { flag.clear(std::memory_order_release); }

        /**
         * @brief Attempts to acquire the spinlock without spinning.
         *
         * Returns true if the lock was acquired successfully, false otherwise.
         * This function does not block the calling thread.
         *
         * @return true if the lock was acquired, false otherwise.
         */
        [[nodiscard]] bool try_lock() { return !flag.test_and_set(std::memory_order_acquire); }

        /**
         * @brief Checks if the spinlock is currently held.
         *
         * Returns true if the lock is currently held by another thread, false otherwise.
         *
         * @return true if the lock is held, false otherwise.
         */
        [[nodiscard]] bool is_locked() const { return flag.test(std::memory_order_acquire); }

    private:
        std::atomic_flag flag = {};
    };
} // namespace BeeEngine::Jobs