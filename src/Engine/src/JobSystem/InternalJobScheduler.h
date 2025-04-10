//
// Created by Aleksandr on 11.03.2024.
//

#pragma once
#include <chrono>
#include <cstddef>
#include <mutex>
#include <queue>
#include <type_traits>
#include <variant>
#include <vector>

#include "Core/TypeDefines.h"
#include "JobScheduler.h"
#include "JobSystem/JobScheduler.h"

namespace BeeEngine
{
    namespace Internal
    {
        class Fiber
        {
        public:
            Fiber(JobWrapper&& job);

            Fiber(Fiber&& other) noexcept;

            // Запускает или возобновляет выполнение файбера
            void Resume();

            bool isCompleted() const { return m_IsCompleted; }

            void MarkIncomplete() { m_IsCompleted = false; }
            void MarkCompleted() { m_IsCompleted = true; }

            JobWrapper GetJob() { return m_Job; }

            boost::context::continuation& GetContext() { return m_Continuation; }

            size_t GetStackSize() const { return m_StackSize; }
            void* GetStackTop() const { return m_StackTop; }
            void* GetStackBottom() const { return m_StackBottom; }

        private:
            JobWrapper m_Job;
            void* m_StackPointer = nullptr;
            void* m_StackTop = nullptr;
            void* m_StackBottom = nullptr;
            size_t m_StackSize = 0;
            boost::context::continuation m_Continuation;
            std::atomic<bool> m_IsCompleted = false;
        };
        class JobScheduler
        {
            friend void BeeEngine::Jobs::this_job::yield();
            friend bool BeeEngine::Jobs::this_job::IsInJob();
            friend size_t BeeEngine::Jobs::this_job::TotalStackSize();
            friend size_t BeeEngine::Jobs::this_job::AvailableStackSize();
            friend void Jobs::this_job::SleepFor(Time::millisecondsD time);
            struct WaitingContext
            {
                Ref<::BeeEngine::Internal::Fiber> fiber;
                std::variant<::BeeEngine::Jobs::Counter*, std::chrono::high_resolution_clock::time_point> condition;
                WaitingContext(::BeeEngine::Ref<::BeeEngine::Internal::Fiber>&& fiber, Jobs::ConditionType condition);
                WaitingContext(const Ref<::BeeEngine::Internal::Fiber>& fiber, Jobs::ConditionType condition);
            };

        public:
            JobScheduler(uint32_t numberOfThreads = Hardware::GetNumberOfCores());
            ~JobScheduler()
            {
                if (!m_Done)
                    Stop();
            }
            void Schedule(JobWrapper&& job);
            void ScheduleAll(std::ranges::range auto& jobs)
            {
                if (std::ranges::size(jobs) == 0)
                {
                    return;
                }
                std::unique_lock lock(m_QueueMutex);
                for (auto& job : jobs)
                {
                    auto& queue = GetQueue(job.Priority);
                    if (jobs.Counter)
                    {
                        jobs.Counter->Increment();
                    }
                    queue.push(&job);
                }
                m_ConditionVariable.notify_all();
            }
            void ScheduleAll(std::vector<JobWrapper> jobs)
            {
                if (jobs.empty())
                {
                    return;
                }
                std::unique_lock lock(m_QueueMutex);
                for (auto& job : jobs)
                {
                    auto& queue = GetQueue(job.Priority);
                    if (job.Counter)
                    {
                        job.Counter->Increment();
                    }
                    queue.push(job);
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
                for (auto& thread : m_Threads)
                {
                    thread.join();
                }
            }
            void WaitForJobsToComplete(Jobs::Counter& counter);

        private:
            // void Schedule(Ref<Internal::Fiber>&& fiber);

            std::queue<JobWrapper>& GetQueue(Jobs::Priority priority)
            {
                switch (priority)
                {
                    case Jobs::Priority::High:
                        return m_HighPriorityJobs;
                    case Jobs::Priority::Low:
                        return m_LowPriorityJobs;
                    default:
                        return m_MediumPriorityJobs;
                }
            }

            void WorkerThread();
            inline bool AllQueuesEmpty() const
            {
                return m_HighPriorityJobs.empty() && m_MediumPriorityJobs.empty() && m_LowPriorityJobs.empty() &&
                       m_WaitingJobs.empty();
            }
            bool GetNextFiber(Ref<::BeeEngine::Internal::Fiber>& fiber);
            Ref<::BeeEngine::Internal::Fiber> PopJob(std::queue<JobWrapper>& queue);
            std::vector<std::thread> m_Threads;
            std::queue<JobWrapper> m_LowPriorityJobs;
            std::queue<JobWrapper> m_MediumPriorityJobs;
            std::queue<JobWrapper> m_HighPriorityJobs;
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
    } // namespace Internal
} // namespace BeeEngine
