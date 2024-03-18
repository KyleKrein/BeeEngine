//
// Created by Александр Лебедев on 25.06.2023.
//

#pragma once
#include <functional>
#include <deque>
#include <mutex>

#include "JobSystem/SpinLock.h"
#include "JobSystem/JobScheduler.h"
#include "Logging/Log.h"

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
            //BeeCoreTrace("Trying to lock flush. Flush was locked: {0}, Push was locked: {1}",m_FlushLock.is_locked(), m_PushLock.is_locked());
            if(m_FlushLock.try_lock())
            {
                m_DeletionQueue.push_back(std::move(function));
                m_FlushLock.unlock();
                return;
            }
            //BeeCoreTrace("Flush is locked. Locking Push. Flush was locked: {0}, Push was locked: {1}",m_FlushLock.is_locked(), m_PushLock.is_locked());
            std::unique_lock lock(m_PushLock);
            m_WaitQueue.push_back(std::move(function));
        }
        void Flush()
        {
            //BeeCoreTrace("Locking flush. Flush was locked: {0}, Push was locked: {1}",m_FlushLock.is_locked(), m_PushLock.is_locked());
            std::unique_lock lock(m_FlushLock);
            if(false /*Jobs::this_job::IsInJob()*/) //TODO: find issues in Job System/DeletionQueue and uncomment this
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
            m_DeletionQueue.clear();
            //BeeCoreTrace("Locking push. Flush was locked: {0}, Push was locked: {1}",m_FlushLock.is_locked(), m_PushLock.is_locked());
            std::unique_lock lock2(m_PushLock);
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

