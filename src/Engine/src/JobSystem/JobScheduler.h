//
// Created by alexl on 23.11.2023.
//

#pragma once
#include <utility>
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include "Hardware.h"
#include "Core/TypeDefines.h"
#include <boost/context/continuation.hpp>
#include <atomic>
namespace BeeEngine
{
    class JobCounter
    {
    public:
        void Increment()
        {
            ++m_Counter;
        }
        void Decrement()
        {
            --m_Counter;
        }
        bool IsZero() const
        {
            return m_Counter == 0;
        }
    private:
        std::atomic<uint32_t> m_Counter = 0;
    };
    namespace Internal
    {
        class Fiber
        {
        public:
            Fiber() = default;

            Fiber(std::function<void()> func, JobCounter *counter = nullptr)
                    : m_Func(std::move(func)), m_IsCompleted(false), m_Counter(counter)
            {
                if (m_Counter)
                {
                    m_Counter->Increment();
                }
            }

            Fiber(Fiber &&other) noexcept
                    : m_Func(std::move(other.m_Func)), m_Continuation(std::move(other.m_Continuation)),
                      m_IsCompleted(other.m_IsCompleted.load()), m_Counter(other.m_Counter)
            {
                other.m_Counter = nullptr;
            }

            Fiber &operator=(Fiber &&other) noexcept
            {
                m_Func = std::move(other.m_Func);
                m_Continuation = std::move(other.m_Continuation);
                m_IsCompleted = other.m_IsCompleted.load();
                m_Counter = other.m_Counter;
                other.m_Counter = nullptr;
                return *this;
            }

            // Запускает или возобновляет выполнение файбера
            void Resume(boost::context::continuation& mainContext)
            {
                if (!m_Continuation)
                {
                    // Первый запуск
                    m_Continuation = boost::context::callcc([this](boost::context::continuation &&c)
                        {
                            m_Continuation = std::move(c);
                            m_IsCompleted = true;
                            m_Func();
                            if (m_Counter && m_IsCompleted)
                            {
                                m_Counter->Decrement();
                            }
                            return std::move(m_Continuation);
                        });
                }
                else
                {
                    // Возобновление
                    m_IsCompleted = true;
                    m_Continuation.resume();
                }
            }

            bool isCompleted() const
            {
                return m_IsCompleted;
            }

            void MarkIncomplete()
            {
                m_IsCompleted = false;
            }
            void MarkCompleted()
            {
                m_IsCompleted = true;
            }

            boost::context::continuation &GetContext()
            {
                return m_Continuation;
            }

        private:
            std::function<void()> m_Func;
            boost::context::continuation m_Continuation;
            std::atomic<bool> m_IsCompleted = false;
            JobCounter *m_Counter = nullptr;
        };
    } // Internal
    class JobScheduler
    {
        struct WaitingContext
        {
            Ref<Internal::Fiber> fiber;
            JobCounter* counter;
        };
    public:
        using JobFunc = std::function<void()>;
        enum class Priority
        {
            Low,
            Normal,
            High
        };
        JobScheduler(uint32_t numberOfThreads = Hardware::GetNumberOfCores())
        {
            for (uint32_t i = 0; i < numberOfThreads; ++i)
            {
                m_Threads.emplace_back([this] { WorkerThread(); });
            }
        }
        void Schedule(JobFunc job, Priority priority = Priority::Normal)
        {
            Ref<Internal::Fiber> fiber = CreateRef<Internal::Fiber>(std::move(job));
            {
                std::lock_guard<std::mutex> lock(m_QueueMutex);
                GetQueue(priority).emplace(std::move(fiber));
            }
            m_ConditionVariable.notify_one();
        }
        void Schedule(Internal::Fiber&& fiber, Priority priority = Priority::Normal)
        {
            {
                std::lock_guard<std::mutex> lock(m_QueueMutex);
                GetQueue(priority).emplace(std::move(fiber));
            }
            m_ConditionVariable.notify_one();
        }
        void Schedule(JobFunc job, JobCounter& counter, Priority priority = Priority::Normal)
        {
            Ref<Internal::Fiber> fiber = CreateRef<Internal::Fiber>(std::move(job), &counter);
            {
                std::lock_guard<std::mutex> lock(m_QueueMutex);
                GetQueue(priority).emplace(std::move(fiber));
            }
            m_ConditionVariable.notify_one();
        }
        void Stop()
        {
            {
                std::lock_guard<std::mutex> lock(m_QueueMutex);
                m_Done = true;
            }
            m_ConditionVariable.notify_all();
        }
        void WaitForJobsToComplete(JobCounter& counter)
        {
            if(counter.IsZero())
            {
                return;
            }
            s_CurrentFiber->MarkIncomplete();
            {
                std::lock_guard<std::mutex> lock(m_WaitingJobsMutex);
                m_WaitingJobs.emplace_back(s_CurrentFiber, &counter);
            }
            boost::context::swap(s_CurrentFiber->GetContext(), s_MainContext);
        }
    private:

        std::queue<Ref<Internal::Fiber>>& GetQueue(Priority priority)
        {
            switch (priority)
            {
                case Priority::High: return m_HighPriorityJobs;
                case Priority::Low: return m_LowPriorityJobs;
                default: return m_MediumPriorityJobs;
            }
        }

        void WorkerThread() {
            s_MainContext = boost::context::callcc([](boost::context::continuation &&c)
                {
                    return std::move(c);
                });
            while (true)
            {
                {
                    std::unique_lock<std::mutex> lock(m_QueueMutex);
                    m_ConditionVariable.wait(lock, [this] { return !AllQueuesEmpty() || m_Done; });

                    if (m_Done && AllQueuesEmpty() && m_WaitingJobs.empty())
                    {
                        break;
                    }

                    if (!GetNextFiber(s_CurrentFiber))
                    {
                        continue;
                    }
                }
                s_CurrentFiber->Resume(s_MainContext);
            }
        }
        bool AllQueuesEmpty() const
        {
            return m_HighPriorityJobs.empty() && m_MediumPriorityJobs.empty() && m_LowPriorityJobs.empty();
        }
        bool GetNextFiber(Ref<Internal::Fiber>& fiber)
        {
            if(!m_WaitingJobs.empty())
            {
                std::unique_lock lock(m_WaitingJobsMutex);
                for (auto it = m_WaitingJobs.begin(); it != m_WaitingJobs.end(); ++it)
                {
                    auto& f = *it;
                    if(f.counter->IsZero())
                    {
                        fiber = std::move(f.fiber);
                        m_WaitingJobs.erase(it);
                        return true;
                    }
                }
                return false;
            }
            if (!m_HighPriorityJobs.empty())
            {
                fiber = PopJob(m_HighPriorityJobs);
                return true;
            }
            if (!m_MediumPriorityJobs.empty())
            {
                fiber = PopJob(m_MediumPriorityJobs);
                return true;
            }
            if (!m_LowPriorityJobs.empty())
            {
                fiber = PopJob(m_LowPriorityJobs);
                return true;
            }
            return false;
        }
        Ref<Internal::Fiber> PopJob(std::queue<Ref<Internal::Fiber>>& queue)
        {
            auto job = std::move(queue.front());
            queue.pop();
            return job;
        }
        std::vector<std::thread> m_Threads;
        std::queue<Ref<Internal::Fiber>> m_LowPriorityJobs;
        std::queue<Ref<Internal::Fiber>> m_MediumPriorityJobs;
        std::queue<Ref<Internal::Fiber>> m_HighPriorityJobs;
        std::mutex m_QueueMutex;
        std::vector<WaitingContext> m_WaitingJobs;
        std::mutex m_WaitingJobsMutex;
        std::condition_variable m_ConditionVariable;
        bool m_Done = false;
        thread_local static Ref<Internal::Fiber> s_CurrentFiber;
        thread_local static boost::context::continuation s_MainContext;
    };
} // BeeEngine