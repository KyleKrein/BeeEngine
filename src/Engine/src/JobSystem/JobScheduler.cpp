//
// Created by alexl on 23.11.2023.
//

#include "JobScheduler.h"
#include "Core/CodeSafety/Expects.h"
#include "InternalJobScheduler.h"

#include <boost/context/preallocated.hpp>
#include <boost/context/protected_fixedsize_stack.hpp>
#include <cstdint>
#include <optional>
#undef GetJob
#include <mutex>
#include <queue>

#include "Core/TypeDefines.h"
#include "Platform/Platform.h"
#include <Core/Move.h>

namespace BeeEngine
{
    void Internal::Fiber::Resume()
    {
        if (!m_Continuation)
        {
// Первый запуск
#if !defined(DEBUG)
            boost::context::protected_fixedsize_stack stack_alloc(m_Job.StackSize);
#else
            boost::context::fixedsize_stack stack_alloc(m_Job.StackSize);
#endif
            auto stackContext = stack_alloc.allocate();
            auto func = [this](boost::context::continuation&& c)
            {
                m_Continuation = std::move(c);
                m_IsCompleted = true;
                m_Job();
                if (m_Job.Counter && m_IsCompleted)
                {
                    m_Job.Counter->Decrement();
                }
                return std::move(m_Continuation);
            };
            boost::context::preallocated prealloc{stackContext.sp, stackContext.size, stackContext};

            using Record =
                boost::context::detail::record<boost::context::continuation, decltype(stack_alloc), decltype(func)>;
            // reserve space for control structure
            m_StackPointer = stackContext.sp;
            void* storage = reinterpret_cast<void*>(
                (reinterpret_cast<uintptr_t>(m_StackPointer) - static_cast<uintptr_t>(sizeof(Record))) &
                ~static_cast<uintptr_t>(0xff));
            // 64byte gab between control structure and stack top
            m_StackTop = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(storage) - static_cast<uintptr_t>(64));
            m_StackBottom = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(m_StackPointer) -
                                                    static_cast<uintptr_t>(stackContext.size));
            // create fast-context
            const std::size_t size =
                reinterpret_cast<uintptr_t>(m_StackTop) - reinterpret_cast<uintptr_t>(m_StackBottom);
            m_StackSize = size;
            m_Continuation = boost::context::callcc(std::allocator_arg, prealloc, stack_alloc, func);
        }
        else
        {
            // Возобновление
            m_IsCompleted = true;
            m_Continuation = m_Continuation.resume();
        }
    }

    Internal::Fiber::Fiber(JobWrapper&& job) : m_Job(BeeMove(job)), m_IsCompleted(false)
    {
        /*if (auto counter = job->Counter)
        {
            counter->Increment();
        }*/
    }

    void Internal::JobScheduler::Schedule(JobWrapper&& job)
    {
        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);
            auto& queue = GetQueue(job.Priority);
            if (job.Counter)
            {
                job.Counter->Increment();
            }
            queue.push(BeeMove(job));
        }
        m_ConditionVariable.notify_one();
    }

    void Internal::JobScheduler::WorkerThread()
    {
        GetMainContext() = boost::context::callcc(
            [this](boost::context::continuation&& c)
            {
                BeeCoreTrace("JobScheduler::WorkerThread: Starting thread {0}", std::this_thread::get_id());
                GetMainContext() = std::move(c);
                while (!m_Done)
                {
                    {
                        std::unique_lock lock(m_QueueMutex);
                        // BeeCoreTrace("Waiting on job {0}", std::this_thread::get_id());
                        m_ConditionVariable.wait(lock, [this] { return !AllQueuesEmpty() || m_Done; });
                        if (AllQueuesEmpty() || m_Done)
                        {
                            break;
                        }

                        if (!GetNextFiber(GetCurrentFiber()))
                        {
                            continue;
                        }
                    }
                    // BeeCoreTrace("Starting job {0}", std::this_thread::get_id());
                    GetCurrentFiber()->Resume();
                }
                BeeCoreTrace("JobScheduler::WorkerThread: Exiting thread {0}", std::this_thread::get_id());
                return std::move(GetMainContext());
            });
    }
    void Internal::Schedule(JobWrapper&& job)
    {
        Internal::Job::s_Instance->Schedule(BeeMove(job));
    }
    void Internal::ScheduleAll(std::vector<JobWrapper> jobs)
    {
        Internal::Job::s_Instance->ScheduleAll(BeeMove(jobs));
    }
    void Internal::Job::Initialize(uint32_t numberOfThreads)
    {
        s_Instance = new Internal::JobScheduler(numberOfThreads);
    }
    namespace Internal
    {
        Ref<Internal::Fiber> JobScheduler::PopJob(std::queue<JobWrapper>& queue)
        {
            auto job = CreateRef<Internal::Fiber>(std::move(queue.front()));
            queue.pop();
            return job;
        }
    } // namespace Internal

    void Jobs::WaitForJobsToComplete(Jobs::Counter& counter)
    {
        Internal::Job::s_Instance->WaitForJobsToComplete(counter);
    }

    void Internal::Job::Shutdown()
    {
        delete Internal::Job::s_Instance;
    }

    Internal::Fiber::Fiber(BeeEngine::Internal::Fiber&& other) noexcept
        : m_Job(BeeMove(other).m_Job),
          m_Continuation(std::move(other.m_Continuation)),
          m_IsCompleted(other.m_IsCompleted.load())
    {
    }

    Internal::JobScheduler::WaitingContext::WaitingContext(BeeEngine::Ref<BeeEngine::Internal::Fiber>&& fiber,
                                                           Jobs::Counter* counter)
        : fiber(BeeMove(fiber)), counter(counter)
    {
    }

    Internal::JobScheduler::WaitingContext::WaitingContext(const BeeEngine::Ref<BeeEngine::Internal::Fiber>& fiber,
                                                           BeeEngine::Jobs::Counter* counter)
        : fiber(fiber), counter(counter)
    {
    }

    Internal::JobScheduler::JobScheduler(uint32_t numberOfThreads)
    {
        for (uint32_t i = 0; i < numberOfThreads; ++i)
        {
            auto& thread = m_Threads.emplace_back([this] { WorkerThread(); });
            ThreadSetAffinity(thread, i);
        }
    }

    void Internal::JobScheduler::WaitForJobsToComplete(Jobs::Counter& counter)
    {
        if (counter.IsZero())
        {
            return;
        }
        auto currentFiber = GetCurrentFiber();
        currentFiber->MarkIncomplete();
        {
            std::unique_lock<std::mutex> lock(m_WaitingJobsMutex);
            m_WaitingJobs.emplace_back(currentFiber, &counter);
        }
        m_ConditionVariable.notify_one();
        currentFiber->GetContext() = std::move(currentFiber->GetContext().resume());
        // GetMainContext() = GetMainContext().resume();
    }

    bool Internal::JobScheduler::GetNextFiber(Ref<BeeEngine::Internal::Fiber>& fiber)
    {
        {
            std::unique_lock lock(m_WaitingJobsMutex);
            if (!m_WaitingJobs.empty())
            {
                for (auto it = m_WaitingJobs.begin(); it != m_WaitingJobs.end(); ++it)
                {
                    if (auto& f = *it; !f.counter || f.counter->IsZero())
                    {
                        fiber = std::move(f.fiber);
                        it = m_WaitingJobs.erase(it);
                        return true;
                    }
                }
            }
        }
        {
            // std::unique_lock lock(m_QueueMutex);
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
        }
        return false;
    }

    void Jobs::this_job::yield()
    {
        auto ptr = Internal::Job::s_Instance->GetCurrentFiber();
        auto& context = ptr->GetContext();
        {
            auto* counter = ptr->GetJob().Counter;
            std::unique_lock lock(Internal::Job::s_Instance->m_WaitingJobsMutex);
            Internal::Job::s_Instance->m_WaitingJobs.emplace_back(std::move(ptr), counter);
        }
        context = std::move(context.resume());
    }
    bool Jobs::this_job::IsInJob()
    {
        return Internal::Job::s_Instance->GetCurrentFiber() != nullptr;
    }
    size_t Jobs::this_job::TotalStackSize()
    {
        BeeExpects(IsInJob());
        return Internal::Job::s_Instance->GetCurrentFiber()->GetStackSize();
    }
    size_t Jobs::this_job::AvailableStackSize()
    {
        BeeExpects(IsInJob());
        char test;
        static const bool growthDown = Internal::Job::s_Instance->GetCurrentFiber()->GetStackBottom() < &test;
        if (growthDown) [[likely]]
        {
            return reinterpret_cast<uintptr_t>(&test) -
                   reinterpret_cast<uintptr_t>(Internal::Job::s_Instance->GetCurrentFiber()->GetStackBottom()) -
                   static_cast<uintptr_t>(1);
        }
        return reinterpret_cast<uintptr_t>(Internal::Job::s_Instance->GetCurrentFiber()->GetStackBottom()) -
               reinterpret_cast<uintptr_t>(&test) - static_cast<uintptr_t>(1);
    }

} // namespace BeeEngine
