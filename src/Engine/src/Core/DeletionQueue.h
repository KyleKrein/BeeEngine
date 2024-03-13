//
// Created by Александр Лебедев on 25.06.2023.
//

#pragma once
#include <functional>
#include <deque>
#include <mutex>

#include "JobSystem/SpinLock.h"
#include "JobSystem/JobScheduler.h"

namespace BeeEngine
{
    class DeletionQueue final
    {
    public:
        static DeletionQueue& Main()
        {
            static DeletionQueue queue;
            return queue;
        }

        static DeletionQueue& Frame()
        {
            static DeletionQueue queue;
            return queue;
        }

        static DeletionQueue& RendererFlush()
        {
            static DeletionQueue queue;
            return queue;
        }

        void PushFunction(std::function<void()>&& function)
        {
            if(m_FlushLock.try_lock())
            {
                m_DeletionQueue.push_back(std::move(function));
                m_FlushLock.unlock();
                return;
            }
            std::unique_lock lock(m_PushLock);
            m_WaitQueue.push_back(std::move(function));
        }
        void Flush()
        {
            std::unique_lock lock(m_FlushLock);
            if(Jobs::this_job::IsInJob())
            {
                Jobs::Counter counter;
                std::vector<Job> jobs;
                for(auto& function : m_DeletionQueue)
                {
                    Job job = {[](void* data)
                    {
                        auto* function = (std::function<void()>*)data;
                        (*function)();
                    }, &function, &counter};
                    jobs.push_back(job);
                }
                Job::ScheduleAll(jobs.data(), jobs.size());
                Job::WaitForJobsToComplete(counter);
            }
            else
            {
                for (auto& function : m_DeletionQueue)
                {
                    function();
                }
            }
            std::unique_lock lock2(m_PushLock);
            m_DeletionQueue.clear();
            for(auto& function : m_WaitQueue)
            {
                m_DeletionQueue.push_back(std::move(function));
            }
            m_WaitQueue.clear();
        }
        [[nodiscard]] bool IsEmpty()
        {
            std::unique_lock lock(m_FlushLock);
            return m_DeletionQueue.empty();
        }

    private:
        std::deque<std::function<void()>> m_DeletionQueue;
        std::deque<std::function<void()>> m_WaitQueue;
        Jobs::SpinLock m_FlushLock;
        Jobs::SpinLock m_PushLock;
    };
}

