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
    enum class JobPriority
    {
        Low,
        Normal,
        High
    };
    namespace Internal
    {
        class Fiber
        {
        public:
            Fiber() = default;

            Fiber(std::function<void()>&& func, JobCounter *counter = nullptr)
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
                    m_Continuation = m_Continuation.resume();
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
        void ThreadSetAffinity(std::thread& thread, uint32_t core);
        class JobScheduler
        {
            struct WaitingContext
            {
                Ref<Internal::Fiber> fiber;
                JobCounter* counter;
            };
        public:
            using JobFunc = std::function<void()>;
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
            void Schedule(JobFunc&& job, JobPriority priority = JobPriority::Normal)
            {
                Ref<Internal::Fiber> fiber = CreateRef<Internal::Fiber>(std::move(job));
                {
                    std::unique_lock<std::mutex> lock(m_QueueMutex);
                    auto& queue = GetQueue(priority);
                    queue.push(std::move(fiber));
                }
                m_ConditionVariable.notify_one();
            }
            void Schedule(JobFunc&& job, JobCounter& counter, JobPriority priority = JobPriority::Normal)
            {
                Ref<Internal::Fiber> fiber = CreateRef<Internal::Fiber>(std::move(job), &counter);
                {
                    std::unique_lock<std::mutex> lock(m_QueueMutex);
                    auto& queue = GetQueue(priority);
                    queue.push(std::move(fiber));
                }
                m_ConditionVariable.notify_one();
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
            void WaitForJobsToComplete(JobCounter& counter)
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
            void Schedule(Ref<Internal::Fiber>&& fiber, JobPriority priority = JobPriority::Normal)
            {
                {
                    std::lock_guard<std::mutex> lock(m_QueueMutex);
                    auto& queue = GetQueue(priority);
                    queue.push(std::move(fiber));
                }
                m_ConditionVariable.notify_one();
            }

            std::queue<Ref<Internal::Fiber>>& GetQueue(JobPriority priority)
            {
                switch (priority)
                {
                    case JobPriority::High: return m_HighPriorityJobs;
                    case JobPriority::Low: return m_LowPriorityJobs;
                    default: return m_MediumPriorityJobs;
                }
            }

            void WorkerThread() {
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
                                                                  GetCurrentFiber()->Resume(GetMainContext());
                                                              }
                                                              return std::move(GetMainContext());
                                                          });
            }
            bool AllQueuesEmpty() const
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
                        if(f.counter->IsZero())
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
            Ref<Internal::Fiber>& GetCurrentFiber();
            boost::context::continuation& GetMainContext();
            thread_local static Ref<Internal::Fiber> s_CurrentFiber;
            thread_local static boost::context::continuation s_MainContext;
        };
    } // Internal
    class Job final
    {
    public:
        template<typename Func>
        requires std::is_invocable_v<Func> && std::is_copy_constructible_v<Func>&&
                std::is_convertible_v<std::invoke_result_t<Func>, void>
        static void Schedule(Func&& job, JobPriority priority = JobPriority::Normal)
        {
            s_Instance->Schedule(std::move(job), priority);
        }
        template<typename Func>
        requires std::is_invocable_v<Func> && std::is_copy_constructible_v<Func>&&
                 std::is_convertible_v<std::invoke_result_t<Func>, void>
        static void Schedule(Func&& job, JobCounter& counter, JobPriority priority = JobPriority::Normal)
        {
            s_Instance->Schedule(std::move(job), counter, priority);
        }

        template<typename Func>
        requires std::is_invocable_v<Func> && (!std::is_copy_constructible_v<Func>)&&
                 std::is_convertible_v<std::invoke_result_t<Func>, void>
        static void Schedule(Func&& job, JobCounter& counter, JobPriority priority = JobPriority::Normal)
        {
            Ref<Func> func = CreateRef<Func>(std::move(job));
            s_Instance->Schedule([func]() { (*func)(); }, counter, priority);
        }
        template<typename Func>
        requires std::is_invocable_v<Func> && (!std::is_copy_constructible_v<Func>)&&
                 std::is_convertible_v<std::invoke_result_t<Func>, void>
        static void Schedule(Func&& job, JobPriority priority = JobPriority::Normal)
        {
            Ref<Func> func = CreateRef<Func>(std::move(job));
            s_Instance->Schedule([func]() { (*func)(); }, priority);
        }

        template<std::ranges::range Container, typename Func>
        static void ForEach(Container& c, Func f, JobPriority priority = JobPriority::Normal)
        {
            for (auto& item : c)
            {
                Job::Schedule([&item, &f]()
                {
                    f(item);
                }, priority);
            }
        }
        template<std::ranges::range Container, typename Func>
        static void ForEach(Container& c, Func f, JobCounter& counter, JobPriority priority = JobPriority::Normal)
        {
            for (auto& item : c)
            {
                Job::Schedule([&item, &f]()
                              {
                                  f(item);
                              }, counter, priority);
            }
        }
        static void WaitForJobsToComplete(JobCounter& counter)
        {
            s_Instance->WaitForJobsToComplete(counter);
        }
    private:
        friend int Main(int argc, char *argv[]);
        static void Initialize(uint32_t numberOfThreads = Hardware::GetNumberOfCores())
        {
            s_Instance = new Internal::JobScheduler(numberOfThreads);
        }
        static void Shutdown()
        {
            delete s_Instance;
        }
        static Internal::JobScheduler* s_Instance;
    };
} // BeeEngine