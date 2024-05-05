//
// Created by alexl on 23.11.2023.
//

#pragma once
#include "Hardware.h"
#include <atomic>
#include <boost/context/continuation.hpp>
#include <memory>
#include <thread>

namespace BeeEngine
{
    namespace Jobs
    {
        using ID = uint32_t;
        namespace this_job
        {
            void yield();
            // inline Jobs::ID GetID();
            bool IsInJob();
        }; // namespace this_job

        class Counter
        {
        public:
            void Increment() { ++m_Counter; }

            void Decrement() { --m_Counter; }

            bool IsZero() const { return m_Counter == 0; }

        private:
            std::atomic<uint32_t> m_Counter = 0;
        };
        enum class Priority
        {
            Low,
            Normal,
            High
        };
    } // namespace Jobs
    namespace Internal
    {
        class JobScheduler;
    }
    class Job final
    {
        friend bool BeeEngine::Jobs::this_job::IsInJob();

    public:
        // Data for job
        using Func = void (*)(void*);
        Func Function;
        void* Data = nullptr;
        Jobs::Counter* Counter = nullptr;
        Jobs::Priority Priority = Jobs::Priority::Normal;
        size_t StackSize = 64 * 1024;

    public:
        // static functions
        static void Schedule(Job& job);
        // static void ScheduleAll(std::ranges::range auto& jobs);
        static void ScheduleAll(Job* jobs, size_t count);
        static void WaitForJobsToComplete(Jobs::Counter& counter);

    private:
        friend void Jobs::this_job::yield();
        friend int Main(int argc, char* argv[]);
        static void Initialize(uint32_t numberOfThreads = Hardware::GetNumberOfCores());
        static void Shutdown();
        static Internal::JobScheduler* s_Instance;
    };

    /*inline void Job::ScheduleAll(std::ranges::range auto& jobs)
    {
        s_Instance->ScheduleAll(jobs);
    }*/
} // namespace BeeEngine