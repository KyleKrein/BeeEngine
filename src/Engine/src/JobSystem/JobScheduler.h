//
// Created by alexl on 23.11.2023.
//

#pragma once
#include "Hardware.h"
#include <atomic>
#include <boost/context/continuation.hpp>
#include <concepts>
#include <memory>
#include <span>
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
        struct Job
        {
            static void Initialize(uint32_t numberOfThreads = Hardware::GetNumberOfCores());
            static void Shutdown();
            static JobScheduler* s_Instance;
        };
        // Вспомогательная структура для хранения аргументов по значению
        template <typename T>
        struct Storage
        {
            using Type = T;
            T value;

            constexpr Storage(T&& v) : value(std::forward<T>(v)) {}
            constexpr T& get() { return value; }
        };

        // Вспомогательная структура для хранения аргументов по ссылке
        template <typename T>
        struct Storage<T&>
        {
            using Type = T&;
            T* value;

            constexpr Storage(T& v) : value(&v) {}
            constexpr T& get() { return *value; }
        };

        // Вспомогательная структура для хранения аргументов по rvalue-ссылке
        template <typename T>
        struct Storage<T&&>
        {
            using Type = T;
            T value;

            constexpr Storage(T&& v) : value(std::move(v)) {}
            constexpr T& get() { return value; }
        };
    } // namespace Internal
    namespace Jobs
    {
        template <typename F, typename... Args>
        class Job final
        {
            friend bool BeeEngine::Jobs::this_job::IsInJob();

        public:
            constexpr Job(Jobs::Counter* counter, Jobs::Priority priority, size_t stackSize, F&& func, Args&&... args)
                : m_Counter(counter),
                  m_Func(std::forward<F>(func)),
                  m_Args(Internal::Storage<Args>(std::forward<Args>(args))...),
                  m_Priority(priority),
                  m_StackSize(stackSize)
            {
            }
            constexpr Jobs::Priority GetPriority() const { return m_Priority; }
            constexpr size_t GetStackSize() const { return m_StackSize; }
            constexpr void SetPriority(Jobs::Priority priority) { m_Priority = priority; }
            constexpr void SetStackSize(size_t stackSize) { m_StackSize = stackSize; }
            constexpr Jobs::Counter* GetCounter() const { return m_Counter; }
            void Call() { CallImpl(std::index_sequence_for<Args...>{}); }

        private:
            Jobs::Counter* m_Counter;
            F m_Func;
            std::tuple<Internal::Storage<Args>...> m_Args;

            Jobs::Priority m_Priority = Jobs::Priority::Normal;
            size_t m_StackSize;

            template <std::size_t... Indices>
            constexpr void CallImpl(std::index_sequence<Indices...>)
            {
                std::invoke(m_Func, std::get<Indices>(m_Args).get()...);
            }
        };
    } // namespace Jobs

    namespace Internal
    {
        struct JobWrapper
        {
            JobWrapper(std::function<void()>&& functionToCallJob,
                       BeeEngine::Jobs::Priority priority,
                       size_t stackSize,
                       Jobs::Counter* counter)
                : Counter(counter),
                  FunctionToCallJob(std::move(functionToCallJob)),
                  Priority(priority),
                  StackSize(stackSize){};
            Jobs::Counter* Counter;
            const std::function<void()> FunctionToCallJob;
            BeeEngine::Jobs::Priority Priority;
            size_t StackSize;
            void operator()() { FunctionToCallJob(); }
        };

        JobWrapper WrapJob(auto&& job)
        {
            Jobs::Priority priority = job.GetPriority();
            size_t stackSize = job.GetStackSize();
            Jobs::Counter* counter = job.GetCounter();
            return JobWrapper([job = std::move(job)]() mutable { job.Call(); }, priority, stackSize, counter);
        }
        void Schedule(JobWrapper&& job);
        void ScheduleAll(std::vector<JobWrapper> jobs);
    } // namespace Internal

    namespace Jobs
    {
        template <typename F, typename... Args>
        void Schedule(Job<F, Args...>& job)
        {
            size_t stackSize = job.GetStackSize();
            Jobs::Priority priority = job.GetPriority();
            Jobs::Counter* counter = job.GetCounter();
            auto callable = [&job]() mutable { job.Call(); };
            Internal::Schedule({callable, priority, stackSize, counter});
        }
        template <typename F, typename... Args>
        void Schedule(Job<F, Args...>&& job)
        {
            Internal::Schedule(Internal::WrapJob(std::move(job)));
        }
        template <typename Job_T>
        void ScheduleAll(std::span<Job_T> jobs)
        {
            std::vector<Internal::JobWrapper> jobWrappers;
            jobWrappers.reserve(jobs.size());
            for (auto& job : jobs)
            {
                jobWrappers.emplace_back(Internal::WrapJob(std::move(job)));
            }
            Internal::ScheduleAll(jobWrappers);
        }
        void WaitForJobsToComplete(Jobs::Counter& counter);
        constexpr size_t DefaultStackSize = 1024 * 64; // in bytes

        template <typename F, typename... Args>
        constexpr auto CreateJob(F&& func, Args&&... args)
        {
            return Job<F, Args...>(
                nullptr, Jobs::Priority::Normal, DefaultStackSize, std::forward<F>(func), std::forward<Args>(args)...);
        }

        template <Jobs::Priority Priority, std::size_t StackSize, typename F, typename... Args>
        constexpr auto CreateJob(F&& func, Args&&... args)
        {
            return Job<F, Args...>(nullptr, Priority, StackSize, std::forward<F>(func), std::forward<Args>(args)...);
        }
        template <Jobs::Priority Priority, std::size_t StackSize, typename F, typename... Args>
        constexpr auto CreateJob(Jobs::Counter& counter, F&& func, Args&&... args)
        {
            return Job<F, Args...>(&counter, Priority, StackSize, std::forward<F>(func), std::forward<Args>(args)...);
        }
        template <typename F, typename... Args>
        constexpr auto
        CreateJob(Jobs::Counter& counter, Jobs::Priority priority, size_t stackSize, F&& func, Args&&... args)
        {
            return Job<F, Args...>(&counter, priority, stackSize, std::forward<F>(func), std::forward<Args>(args)...);
        }
        template <typename F, typename... Args>
        constexpr auto CreateJob(Jobs::Counter& counter, Jobs::Priority priority, F&& func, Args&&... args)
        {
            return Job<F, Args...>(
                &counter, priority, DefaultStackSize, std::forward<F>(func), std::forward<Args>(args)...);
        }
        template <typename F, typename... Args>
        constexpr auto CreateJob(Jobs::Counter& counter, F&& func, Args&&... args)
        {
            return Job<F, Args...>(
                &counter, Priority::Normal, DefaultStackSize, std::forward<F>(func), std::forward<Args>(args)...);
        }
    } // namespace Jobs

    /*inline void Job::ScheduleAll(std::ranges::range auto& jobs)
    {
        s_Instance->ScheduleAll(jobs);
    }*/
} // namespace BeeEngine