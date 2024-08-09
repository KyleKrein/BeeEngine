//
// Created by alexl on 23.11.2023.
//

#pragma once
#include "Core/Time.h"
#include "Hardware.h"
#include <Core/Move.h>
#include <array>
#include <atomic>
#include <boost/context/continuation.hpp>
#include <concepts>
#include <cstddef>
#include <functional>
#include <memory>
#include <ranges>
#include <span>
#include <thread>
#include <utility>
#include <variant>
#include <vector>


namespace BeeEngine
{
    namespace Jobs
    {
        using ID = uint32_t;
        namespace this_job
        {
            /**
             * @brief Yields the current job, allowing other jobs to run.
             * This function is useful when a job needs to give up control temporarily,
             * allowing other jobs to execute. It is a non-blocking operation.
             *
             * @return void
             *
             * @note This function should be called only within a job.
             *       Calling it outside of a job may lead to unexpected behavior.
             */
            void yield();
            /**
             * @brief Returns total size of the
             * current stack in bytes. Must be called
             * only in a job
             * @return size_t
             */
            size_t TotalStackSize();
            /**
             * @brief Returns approximate size of the
             * left space on the current stack in bytes.
             * Must be called only in a job
             * @return size_t
             */
            size_t AvailableStackSize();
            /**
             * @brief Suspends this job for at least
             * amount of time, that was specified.
             * It is recommended to use std::chrono::literals
             * to pass a time parameter
             * @param time
             */
            void SleepFor(Time::millisecondsD time);
            // inline Jobs::ID GetID();
            /**
             * @brief Checks if the current code is executing within a job.
             *
             * @return bool
             * @retval true  The current code is executing within a job.
             * @retval false The current code is not executing within a job.
             */
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
        using ConditionType = std::variant<::BeeEngine::Jobs::Counter*, std::chrono::high_resolution_clock::time_point>;
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
        /**
         * @brief Waits for all jobs associated with the given counter to complete.
         * If in a job, yields the current job, allowing other jobs to run.
         * If not in a job, blocks the current thread until all jobs associated with the given counter have finished
         * executing.
         *
         * @param counter A valid reference to the Jobs::Counter object associated with the jobs to wait for.
         *
         * @return void
         *
         * @note This function is thread-safe and can be called from multiple threads simultaneously.
         *
         * @note If a job associated with the given counter calls WaitForJobsToComplete on the same counter,
         *       a deadlock may occur. It is recommended to avoid such circular dependencies.
         */
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
        template <typename Func, typename... Args>
        constexpr auto ForEach(std::ranges::range auto&& container, Jobs::Counter& counter, Func&& func, Args&&... args)
        {
            size_t maxNumber = container.size();
            if (maxNumber == 0)
            {
                return;
            }
            const size_t cores = Hardware::GetNumberOfCores();
            const size_t remainder = maxNumber % cores;
            const size_t jobsPerThread = maxNumber / cores;
            size_t start = 0;
            for (size_t core = 0; core < cores; ++core)
            {
                size_t end = start + jobsPerThread + (core < remainder ? 1 : 0);
                auto job = CreateJob(
                    counter,
                    [start, end, &container](Func&& func, Args&&... args) mutable
                    {
                        for (size_t i = start; i < end; ++i)
                        {
                            func(container[i], std::move(i), std::forward<Args>(args)...);
                        }
                    },
                    func,
                    args...);
                Schedule(BeeMove(job));
                start = end;
            }
        }
        template <typename Func, typename... Args>
        constexpr auto ForEach(std::ranges::range auto&& container, Func&& func, Args&&... args)
        {
            return ForEach(container,
                           *static_cast<Jobs::Counter*>(nullptr),
                           std::forward<Func>(func),
                           std::forward<Args>(args)...);
        }
    } // namespace Jobs

    /*inline void Job::ScheduleAll(std::ranges::range auto& jobs)
    {
        s_Instance->ScheduleAll(jobs);
    }*/
} // namespace BeeEngine