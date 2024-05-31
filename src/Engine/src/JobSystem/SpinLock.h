//
// Created by alexl on 26.11.2023.
//

#pragma once
#include <atomic>
namespace BeeEngine::Jobs
{
    class SpinLock
    {
    public:
        void lock()
        {
            while (flag.test_and_set(std::memory_order_acquire))
            {
                // Активное ожидание (вращение)
            }
        }

        void unlock() { flag.clear(std::memory_order_release); }

        [[nodiscard]] bool try_lock() { return !flag.test_and_set(std::memory_order_acquire); }

        [[nodiscard]] bool is_locked() const { return flag.test(std::memory_order_acquire); }

    private:
        std::atomic_flag flag = {};
    };
} // namespace BeeEngine::Jobs