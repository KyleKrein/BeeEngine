//
// Created by alexl on 23.11.2023.
//

#include "JobScheduler.h"

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
        if (auto counter = job->Counter)
        {
            counter->Increment();
        }
    }

    void Internal::JobScheduler::Schedule(Job *job)
    {
        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);
            auto& queue = GetQueue(job->Priority);
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
}