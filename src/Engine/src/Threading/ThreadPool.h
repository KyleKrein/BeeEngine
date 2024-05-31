//
// Created by Александр Лебедев on 13.08.2023.
//
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
#include <version>
#pragma once

// Thread pool was originally taken from https://github.com/ZenSepiol/Dear-ImGui-App-Framework/tree/main
// and modified to fit the needs of the engine

namespace BeeEngine
{
    class ThreadPool
    {
    public:
        ThreadPool(const size_t size)
            : m_BusyThreads(size), m_Threads(std::vector<std::thread>(size)), m_ShutdownRequested(false)
        {
            for (size_t i = 0; i < size; ++i)
            {
                m_Threads[i] = std::thread(ThreadWorker(this));
            }
        }

        ~ThreadPool() { Shutdown(); }

        ThreadPool(const ThreadPool&) = delete;
        ThreadPool(ThreadPool&&) = delete;

        ThreadPool& operator=(const ThreadPool&) = delete;
        ThreadPool& operator=(ThreadPool&&) = delete;

        // Waits until threads finish their current task and shutdowns the pool
        void Shutdown()
        {
            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                m_ShutdownRequested = true;
                m_ConditionVariable.notify_all();
            }

            for (auto& thread : m_Threads)
            {
                if (thread.joinable())
                {
                    thread.join();
                }
            }
        }

        template <typename F, typename... Args>
        auto AddTask(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
        {
#if __cpp_lib_move_only_function
            std::packaged_task<decltype(f(args...))()> task(std::forward<F>(f), std::forward<Args>(args)...);
            auto future = task.get_future();
            auto wrapperFunc = [task = std::move(task)]() mutable { std::move(task)(); };
            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                m_Queue.push(std::move(wrapperFunc));
                // Wake up one thread if its waiting
                m_ConditionVariable.notify_one();
            }

            // Return future from promise
            return future;
#else

            auto taskPtr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(std::forward<F>(f),
                                                                                        std::forward<Args>(args)...);

            auto wrapperFunc = [taskPtr]() { (*taskPtr)(); };
            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                m_Queue.push(wrapperFunc);
                // Wake up one thread if its waiting
                m_ConditionVariable.notify_one();
            }

            // Return future from promise
            return taskPtr->get_future();
#endif
        }

        size_t QueueSize()
        {
            std::unique_lock<std::mutex> lock(m_Mutex);
            return m_Queue.size();
        }

    private:
        class ThreadWorker
        {
        public:
            ThreadWorker(ThreadPool* pool) : m_ThreadPool(pool) {}

            void operator()()
            {
                std::unique_lock<std::mutex> lock(m_ThreadPool->m_Mutex);
                while (!m_ThreadPool->m_ShutdownRequested ||
                       (m_ThreadPool->m_ShutdownRequested && !m_ThreadPool->m_Queue.empty()))
                {
                    m_ThreadPool->m_BusyThreads--;
                    m_ThreadPool->m_ConditionVariable.wait(lock,
                                                           [this] {
                                                               return this->m_ThreadPool->m_ShutdownRequested ||
                                                                      !this->m_ThreadPool->m_Queue.empty();
                                                           });
                    m_ThreadPool->m_BusyThreads++;

                    if (!this->m_ThreadPool->m_Queue.empty())
                    {
#if __cpp_lib_move_only_function
                        auto func = std::move(m_ThreadPool->m_Queue.front());
#else
                        auto func = m_ThreadPool->m_Queue.front();
#endif
                        m_ThreadPool->m_Queue.pop();

                        lock.unlock();
                        func();
                        lock.lock();
                    }
                }
            }

        private:
            ThreadPool* m_ThreadPool;
        };

    public:
        size_t BusyThreads() const { return m_BusyThreads; }

    private:
        size_t m_BusyThreads;
        mutable std::mutex m_Mutex;
        std::condition_variable m_ConditionVariable;

        std::vector<std::thread> m_Threads;
        bool m_ShutdownRequested;

#if __cpp_lib_move_only_function
        std::queue<std::move_only_function<void()>> m_Queue;
#else
        std::queue<std::function<void()>> m_Queue;
#endif
    };
} // namespace BeeEngine