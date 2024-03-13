//
// Created by alexl on 23.11.2023.
//

#include "JobScheduler.h"
#include "InternalJobScheduler.h"

#include <mutex>
#include <queue>

#include "Core/TypeDefines.h"

namespace BeeEngine
{
    void Internal::Fiber::Resume()
    {
        if (!m_Continuation)
        {
            // Первый запуск
            boost::context::fixedsize_stack stack_alloc(m_Job->StackSize);
            m_Continuation = boost::context::callcc(std::allocator_arg, stack_alloc,[this](boost::context::continuation &&c)
                                                    {
                                                        m_Continuation = std::move(c);
                                                        m_IsCompleted = true;
                                                        m_Job->Function(m_Job->Data);
                                                        if (m_Job->Counter && m_IsCompleted)
                                                        {
                                                            m_Job->Counter->Decrement();
                                                        }
                                                        return std::move(m_Continuation);
                                                    });
        }
        else
        {
            // Возобновление
            m_IsCompleted = true;
            m_Continuation = m_Continuation.resume();
        }
    }

    Internal::Fiber::Fiber(Job *job)
            : m_Job(job), m_IsCompleted(false)
    {
        /*if (auto counter = job->Counter)
        {
            counter->Increment();
        }*/
    }

    void Internal::JobScheduler::Schedule(Job *job)
    {
        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);
            auto& queue = GetQueue(job->Priority);
            if(job->Counter)
            {
                job->Counter->Increment();
            }
            queue.push(job);
        }
        m_ConditionVariable.notify_one();
    }

    void Internal::JobScheduler::WorkerThread()
    {
        GetMainContext() = boost::context::callcc([this](boost::context::continuation &&c)
                                                  {
                                                      GetMainContext() = std::move(c);
                                                      while (!m_Done)
                                                      {
                                                          {
                                                              std::unique_lock<std::mutex> lock(m_QueueMutex);
                                                              m_ConditionVariable.wait(lock, [this] { return !AllQueuesEmpty() || m_Done; });

                                                              if (AllQueuesEmpty() && m_WaitingJobs.empty())
                                                              {
                                                                  break;
                                                              }

                                                              if (!GetNextFiber(GetCurrentFiber()))
                                                              {
                                                                  continue;
                                                              }
                                                          }
                                                          GetCurrentFiber()->Resume();
                                                      }
                                                      return std::move(GetMainContext());
                                                  });
    }
    void Job::Initialize(uint32_t numberOfThreads)
    {
        s_Instance = new Internal::JobScheduler(numberOfThreads);
    }

    void Job::Schedule(Job &job)
    {
        s_Instance->Schedule(&job);
    }
    namespace Internal
    {
        Ref<Internal::Fiber> JobScheduler::PopJob(std::queue<Job *> &queue)
        {
            auto job = CreateRef<Internal::Fiber>(queue.front());
            queue.pop();
            return job;
        }
    }

    void Job::WaitForJobsToComplete(Jobs::Counter &counter)
    {
        s_Instance->WaitForJobsToComplete(counter);
    }

    void Job::Shutdown()
    {
        delete s_Instance;
    }

    Internal::Fiber::Fiber(BeeEngine::Internal::Fiber&& other) noexcept
    : m_Continuation(std::move(other.m_Continuation)),
                      m_IsCompleted(other.m_IsCompleted.load())
    {
        std::swap(m_Job, other.m_Job);
    }

    BeeEngine::Internal::Fiber& Internal::Fiber::operator=(BeeEngine::Internal::Fiber&& other) noexcept
    {
        std::swap(m_Job, other.m_Job);
        m_Continuation = std::move(other.m_Continuation);
        m_IsCompleted = other.m_IsCompleted.load();
        return *this;
    }

    Internal::JobScheduler::WaitingContext::WaitingContext(BeeEngine::Ref<BeeEngine::Internal::Fiber>&& fiber, Jobs::Counter* counter)
    : fiber(std::move(fiber)), counter(counter)
    {}

    Internal::JobScheduler::WaitingContext::WaitingContext(const BeeEngine::Ref<BeeEngine::Internal::Fiber>& fiber, BeeEngine::Jobs::Counter* counter)
    : fiber(fiber), counter(counter)
    {}

    Internal::JobScheduler::JobScheduler(uint32_t numberOfThreads)
    {
        for (uint32_t i = 0; i < numberOfThreads; ++i)
        {
            auto& thread = m_Threads.emplace_back([this] { WorkerThread(); });
            Internal::ThreadSetAffinity(thread, i);
        }
    }

    void Internal::JobScheduler::WaitForJobsToComplete(Jobs::Counter& counter)
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

    bool Internal::JobScheduler::GetNextFiber(Ref<BeeEngine::Internal::Fiber>& fiber)
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


    void Job::ScheduleAll(Job* jobs, size_t count)
    {
        s_Instance->ScheduleAll(jobs, count);
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
    bool Jobs::this_job::IsInJob()
    {
        return Job::s_Instance->GetCurrentFiber() != nullptr;
    }

}
