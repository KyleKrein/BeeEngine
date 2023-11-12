//
// Created by alexl on 11.11.2023.
//

#pragma once

#include <coroutine>
#include <exception>
#include <future>
#include <functional>
#include <thread>

namespace BeeEngine
{
    template<typename T = void>
    class Task
    {
    public:
        struct promise_type;
        using handle_type = std::coroutine_handle<promise_type>;

        Task(handle_type h) : m_Handle(h)
        {}

        ~Task()
        { if (m_Handle) m_Handle.destroy(); }

        Task(const Task &) = delete;

        Task &operator=(const Task &) = delete;

        Task(Task &&t) noexcept: m_Handle(t.m_Handle)
        { t.m_Handle = nullptr; }

        Task &operator=(Task &&t) noexcept
        {
            if (std::addressof(t) != this)
            {
                if (m_Handle) m_Handle.destroy();
                m_Handle = t.m_Handle;
                t.m_Handle = nullptr;
            }
            return *this;
        }

        T get()
        {
            if (!m_Handle.done())
                m_Handle.resume();
            return m_Handle.promise().m_Value;
        }

        struct promise_type
        {
            T m_Value;
            std::exception_ptr m_Exception;
            std::coroutine_handle<> m_AwaitingCoroutine;

            Task get_return_object()
            {
                return Task{handle_type::from_promise(*this)};
            }

            std::suspend_always initial_suspend()
            { return {}; }

            std::suspend_always final_suspend() noexcept
            { return {}; }

            void return_value(T &&val)
            {
                m_Value = std::move(val);
            }
            void return_value(const T &val)
            {
                m_Value = val;
            }

            void unhandled_exception()
            { m_Exception = std::current_exception(); }
        };

        bool await_ready() const noexcept {
            return m_Handle.done();
        }

        void await_suspend(std::coroutine_handle<> awaitingCoroutine) noexcept {
            m_Handle.promise().m_AwaitingCoroutine = awaitingCoroutine;
            m_Handle.resume();
        }

        T await_resume() {
            if (m_Handle.promise().m_Exception) {
                std::rethrow_exception(m_Handle.promise().m_Exception);
            }
            if constexpr (!std::is_void<T>::value) {
                return m_Handle.promise().m_Value;
            }
        }

    private:
        handle_type m_Handle;
    };

    template<>
    struct Task<void>
    {
        struct promise_type
        {
            std::exception_ptr m_Exception;
            std::coroutine_handle<> m_AwaitingCoroutine;
            void return_void()
            {}

            Task get_return_object()
            { return {handle_type::from_promise(*this)}; }

            std::suspend_always initial_suspend()
            { return {}; }

            std::suspend_always final_suspend() noexcept
            { return {}; }

            void unhandled_exception()
            {}
        };

        using handle_type = std::coroutine_handle<promise_type>;
        handle_type handle;

        Task(handle_type h) : handle(h)
        {}

        ~Task()
        { if (handle) handle.destroy(); }

        Task(const Task &) = delete;

        Task(Task &&other) : handle(other.handle)
        { other.handle = nullptr; }

        Task &operator=(const Task &) = delete;

        Task &operator=(Task &&other)
        {
            if (this != &other)
            {
                handle = other.handle;
                other.handle = nullptr;
            }
            return *this;
        }

        void get()
        {
            if (!handle.done())
                handle.resume();
        }
        bool await_ready() const noexcept {
            return handle.done();
        }

        void await_suspend(std::coroutine_handle<> awaitingCoroutine) noexcept {
            handle.promise().m_AwaitingCoroutine = awaitingCoroutine;
            handle.resume();
        }

        void await_resume() {
            if (handle.promise().m_Exception) {
                std::rethrow_exception(handle.promise().m_Exception);
            }
        }
    };

    template<typename T = void>
    class AsyncTask {
    public:
        struct promise_type {
            std::promise<T> p;
            std::future<T> get_return_object() {
                return p.get_future();
            }

            std::suspend_never initial_suspend() { return {}; }
            std::suspend_never final_suspend() noexcept { return {}; }

            template<typename U>
            requires (!std::is_void<U>::value)
            void return_value(U&& value) {
                p.set_value(std::forward<U>(value));
            }

            void return_void() {
                p.set_value();
            }

            void unhandled_exception() {
                p.set_exception(std::current_exception());
            }
        };

        using handle_type = std::coroutine_handle<promise_type>;
        std::future<T> result;

        AsyncTask(std::future<T>&& future) : result(std::move(future)) {}
        std::future<T>& get() { return result; }
    };

    template<typename T = void>
    AsyncTask<T> run_async(std::function<T()> func) {
        if constexpr (std::is_void<T>::value) {
            func();
            co_return;
        } else {
            co_return func();
        }
    }

    template<typename T = void>
    T sync_await(Task<T>&& task)
    {
        if constexpr (std::is_void<T>::value) {
            task.get();
        } else {
            return task.get();
        }
    }
}