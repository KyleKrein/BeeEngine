//
// Created by Aleksandr on 11.03.2024.
//

#pragma once
#include <mutex>
#include <queue>

#include "Core/TypeDefines.h"

namespace BeeEngine
{
    namespace Internal
    {
        class Fiber
        {
        public:
            Fiber() = default;

            Fiber(Job* job);

            Fiber(Fiber &&other) noexcept;

            Fiber &operator=(Fiber &&other) noexcept;

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
        class JobScheduler
        {
            friend void BeeEngine::Jobs::this_job::yield();
            friend bool BeeEngine::Jobs::this_job::IsInJob();
            struct WaitingContext
            {
                Ref<::BeeEngine::Internal::Fiber> fiber;
                ::BeeEngine::Jobs::Counter* counter;
                WaitingContext(::BeeEngine::Ref<::BeeEngine::Internal::Fiber>&& fiber, Jobs::Counter* counter);
                WaitingContext(const Ref<::BeeEngine::Internal::Fiber>& fiber, ::BeeEngine::Jobs::Counter* counter);
            };
        public:
            JobScheduler(uint32_t numberOfThreads = Hardware::GetNumberOfCores());
            ~JobScheduler()
            {
                if(!m_Done)
                    Stop();
            }
            void Schedule(Job* job);
            void ScheduleAll(std::ranges::range auto& jobs)
            {
                if(std::ranges::size(jobs) == 0)
                {
                    return;
                }
                std::unique_lock lock(m_QueueMutex);
                for (auto& job : jobs)
                {
                    auto& queue = GetQueue(job.Priority);
                    if(jobs.Counter)
                    {
                        jobs.Counter->Increment();
                    }
                    queue.push(&job);
                }
                m_ConditionVariable.notify_all();
            }
            void ScheduleAll(Job* jobs, size_t count)
            {
                if(count == 0)
                {
                    return;
                }
                std::unique_lock lock(m_QueueMutex);
                for (size_t i = 0; i < count; ++i)
                {
                    auto& queue = GetQueue(jobs[i].Priority);
                    if(jobs[i].Counter)
                    {
                        jobs[i].Counter->Increment();
                    }
                    queue.push(&jobs[i]);
                }
                m_ConditionVariable.notify_all();
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
            void WaitForJobsToComplete(Jobs::Counter& counter);
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
            bool GetNextFiber(Ref<::BeeEngine::Internal::Fiber>& fiber);
            Ref<::BeeEngine::Internal::Fiber> PopJob(std::queue<Job*>& queue);
            std::vector<std::thread> m_Threads;
            std::queue<Job*> m_LowPriorityJobs;
            std::queue<Job*> m_MediumPriorityJobs;
            std::queue<Job*> m_HighPriorityJobs;
            std::mutex m_QueueMutex;
            std::vector<WaitingContext> m_WaitingJobs;
            std::mutex m_WaitingJobsMutex;
            std::condition_variable m_ConditionVariable;
            bool m_Done = false;
            Ref<::BeeEngine::Internal::Fiber>& GetCurrentFiber();
            boost::context::continuation& GetMainContext();
            thread_local static Ref<::BeeEngine::Internal::Fiber> s_CurrentFiber;
            thread_local static boost::context::continuation s_MainContext;
        };
    }
}
