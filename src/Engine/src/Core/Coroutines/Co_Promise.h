//
// Created by alexl on 12.11.2023.
//

#pragma once

#include <coroutine>
#include <memory>
#include <exception>
#include <condition_variable>
#include <mutex>
#include "Core/TypeDefines.h"

namespace BeeEngine
{
    template<typename T>
    class Co_Future;

    template<typename T>
    class Co_Promise
    {
        friend Co_Future<T>;
    public:
        Co_Promise() : state(CreateRef<State>())
        {}

        Co_Future<T> get_future()
        { return Co_Future<T>{state}; }

        void set_value(T value)
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            state->value = std::move(value);
            state->is_ready = true;
            state->cv.notify_all();
        }

        // Определение set_exception и других методов при необходимости

    private:
        struct State
        {
            std::mutex mutex;
            std::condition_variable cv;
            T value;
            bool is_ready = false;
            // Поле для хранения исключения при необходимости
        };

        Ref<State> state;
    };

    template<typename T>
    class Co_Future
    {
    public:
        Co_Future(Ref<typename Co_Promise<T>::State> state) : state(std::move(state))
        {}

        T get()
        {
            std::unique_lock<std::mutex> lock(state->mutex);
            state->cv.wait(lock, [this]
            { return state->is_ready; });
            return std::move(state->value);
        }

        bool await_ready() const noexcept
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            return state->is_ready;
        }

        T await_resume()
        {
            return std::move(state->value);
        }

        void await_suspend(std::coroutine_handle<> handle)
        {
            std::unique_lock<std::mutex> lock(state->mutex);
            state->cv.wait(lock, [this]
            { return state->is_ready; });
            handle.resume();  // Возобновление корутины после получения результата
        }

        // Определение методов для работы с исключениями при необходимости

    private:
        Ref<typename Co_Promise<T>::State> state;
    };

    template<>
    class Co_Promise<void> {
        friend Co_Future<void>;
    public:
        Co_Promise() : state(CreateRef<State>()) {}

        Co_Future<void> get_future();

        void set_value() {
            std::lock_guard<std::mutex> lock(state->mutex);
            state->is_ready = true;
            state->cv.notify_all();
        }

    private:
        struct State {
            std::mutex mutex;
            std::condition_variable cv;
            bool is_ready = false;
        };

        Ref<State> state;
    };

    template<>
    class Co_Future<void> {
    public:
        Co_Future(Ref<typename Co_Promise<void>::State> state)
                : state(std::move(state)) {}

        void get() {
            std::unique_lock<std::mutex> lock(state->mutex);
            state->cv.wait(lock, [this]{ return state->is_ready; });
        }

        bool await_ready() const noexcept {
            std::lock_guard<std::mutex> lock(state->mutex);
            return state->is_ready;
        }

        void await_suspend(std::coroutine_handle<> handle) {
            std::unique_lock<std::mutex> lock(state->mutex);
            state->cv.wait(lock, [this]{ return state->is_ready; });
            handle.resume();
        }

        void await_resume() {
            // Ничего не возвращается, так как тип void
        }

    private:
        Ref<typename Co_Promise<void>::State> state;
    };



    inline Co_Future<void> Co_Promise<void>::get_future()
    {
        return Co_Future<void>{state};
    }


}