//
// Created by alexl on 23.11.2023.
//

#pragma once
#include "Core/TypeDefines.h"
#include <utility>
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include "Hardware.h"
#include <boost/context/continuation.hpp>
#include <atomic>
namespace BeeEngine
{
    namespace Jobs
    {
        namespace this_job
        {
            inline void yield();
        };

        class Counter
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
        enum class Priority
        {
            Low,
            Normal,
            High
        };
    } // Jobs
    namespace Internal
    {
        class JobScheduler;
        class Fiber;
    }
    class Job final
    {
    public:
        //Data for job
        using Func = void(*)(void*);
        Func Function;
        void* Data = nullptr;
        Jobs::Counter* Counter = nullptr;
        Jobs::Priority Priority = Jobs::Priority::Normal;
        size_t StackSize = 64 * 1024;
    public:
        //static functions
        static void Schedule(Job& job);
        static void ScheduleAll(std::ranges::range auto& jobs);
        static void WaitForJobsToComplete(Jobs::Counter& counter);
    private:
        friend void Jobs::this_job::yield();
        friend int Main(int argc, char *argv[]);
        static void Initialize(uint32_t numberOfThreads = Hardware::GetNumberOfCores());
        static void Shutdown();
        static Internal::JobScheduler* s_Instance;
    };
    namespace Internal
    {
        class Fiber
        {
        public:
            Fiber() = default;

            Fiber(Job* job);

            Fiber(Fiber &&other) noexcept
                    : m_Continuation(std::move(other.m_Continuation)),
                      m_IsCompleted(other.m_IsCompleted.load())
            {
                std::swap(m_Job, other.m_Job);
            }

            Fiber &operator=(Fiber &&other) noexcept
            {
                std::swap(m_Job, other.m_Job);
                m_Continuation = std::move(other.m_Continuation);
                m_IsCompleted = other.m_IsCompleted.load();
                return *this;
            }

            // Запускает или возобновляет выполнение файбера
            void Resume();

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

            Job* GetJob()
            {
                return m_Job;
            }

            boost::context::continuation &GetContext()
            {
                return m_Continuation;
            }

        private:
            Job* m_Job = nullptr;
            boost::context::continuation m_Continuation;
            std::atomic<bool> m_IsCompleted = false;
        };
        void ThreadSetAffinity(std::thread& thread, uint32_t core);
        class JobScheduler
        {
            friend void BeeEngine::Jobs::this_job::yield();
            struct WaitingContext
            {
                Ref<Internal::Fiber> fiber;
                Jobs::Counter* counter;
            };
        public:
            JobScheduler(uint32_t numberOfThreads = Hardware::GetNumberOfCores())
            {
                for (uint32_t i = 0; i < numberOfThreads; ++i)
                {
                    auto& thread = m_Threads.emplace_back([this] { WorkerThread(); });
                    Internal::ThreadSetAffinity(thread, i);
                }
            }
            ~JobScheduler()
            {
                if(!m_Done)
                    Stop();
            }
            void Schedule(Job* job);
            void ScheduleAll(std::ranges::range auto& jobs)
            {
                std::unique_lock lock(m_QueueMutex);
                for (auto& job : jobs)
                {
                    auto& queue = GetQueue(job->Priority);
                    queue.push(job);
                }
            }
            void Stop()
            {
                {
                    std::unique_lock<std::mutex> lock(m_QueueMutex);
                    m_Done = true;
                }
                m_ConditionVariable.notify_all();
                for (auto &thread : m_Threads)
                {
                    thread.join();
                }
            }
            void WaitForJobsToComplete(Jobs::Counter& counter)
            {
                if(counter.IsZero())
                {
                    return;
                }
                GetCurrentFiber()->MarkIncomplete();
                {
                    std::unique_lock<std::mutex> lock(m_WaitingJobsMutex);
                    m_WaitingJobs.emplace_back(GetCurrentFiber(), &counter);
                }
                m_ConditionVariable.notify_one();
                GetCurrentFiber()->GetContext() = std::move(GetCurrentFiber()->GetContext().resume());
                //GetMainContext() = GetMainContext().resume();
            }
        private:
            //void Schedule(Ref<Internal::Fiber>&& fiber);

            std::queue<Job*>& GetQueue(Jobs::Priority priority)
            {
                switch (priority)
                {
                    case Jobs::Priority::High: return m_HighPriorityJobs;
                    case Jobs::Priority::Low: return m_LowPriorityJobs;
                    default: return m_MediumPriorityJobs;
                }
            }

            void WorkerThread();
            inline bool AllQueuesEmpty() const
            {
                return m_HighPriorityJobs.empty() && m_MediumPriorityJobs.empty() && m_LowPriorityJobs.empty() &&
                       m_WaitingJobs.empty();
            }
            bool GetNextFiber(Ref<Internal::Fiber>& fiber)
            {
                if(!m_WaitingJobs.empty())
                {
                    std::unique_lock lock(m_WaitingJobsMutex);
                    for (auto it = m_WaitingJobs.begin(); it != m_WaitingJobs.end(); ++it)
                    {
                        auto& f = *it;
                        if(!f.counter || f.counter->IsZero())
                        {
                            fiber = std::move(f.fiber);
                            m_WaitingJobs.erase(it);
                            return true;
                        }
                    }
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
            Ref<Internal::Fiber> PopJob(std::queue<Job*>& queue);
            std::vector<std::thread> m_Threads;
            std::queue<Job*> m_LowPriorityJobs;
            std::queue<Job*> m_MediumPriorityJobs;
            std::queue<Job*> m_HighPriorityJobs;
            std::mutex m_QueueMutex;
            std::vector<WaitingContext> m_WaitingJobs;
            std::mutex m_WaitingJobsMutex;
            std::condition_variable m_ConditionVariable;
            bool m_Done = false;
            Ref<Internal::Fiber>& GetCurrentFiber();
            boost::context::continuation& GetMainContext();
            thread_local static Ref<Internal::Fiber> s_CurrentFiber;
            thread_local static boost::context::continuation s_MainContext;
        };
    } // Internal
    void Job::ScheduleAll(std::ranges::range auto& jobs)
    {
        s_Instance->ScheduleAll(jobs);
    }
    void Jobs::this_job::yield()
    {
        auto ptr = Job::s_Instance->GetCurrentFiber();
        auto& ref = Job::s_Instance->GetCurrentFiber();
        {
            auto* counter = ref->GetJob()->Counter;
            std::unique_lock lock(Job::s_Instance->m_WaitingJobsMutex);
            Job::s_Instance->m_WaitingJobs.emplace_back(std::move(ptr), counter);
        }
        ref->GetContext() = std::move(ref->GetContext().resume());
    }
} // BeeEngine