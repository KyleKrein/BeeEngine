//
// Created by alexl on 12.11.2023.
//

#pragma once
#include <coroutine>
#include <future>
#include <thread>

namespace BeeEngine
{
    template <typename T>
    struct FutureAwaiter
    {
        std::future<T>& future;

        bool await_ready() { return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready; }

        void await_suspend(std::coroutine_handle<> handle)
        {
            std::thread(
                [this, handle]()
                {
                    this->future.wait();
                    handle.resume();
                })
                .detach();
        }

        T await_resume() { return future.get(); }
    };

    template <typename T>
    FutureAwaiter<T> operator co_await(std::future<T>& future)
    {
        return FutureAwaiter<T>{future};
    }
} // namespace BeeEngine