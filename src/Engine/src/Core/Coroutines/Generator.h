//
// Created by alexl on 12.11.2023.
//

#pragma once
#include <coroutine>
#include <exception>
#include <future>
#include <optional>
#include <thread>

namespace BeeEngine
{
    template <typename T>
    class Generator
    {
    public:
        struct promise_type;
        using handle_type = std::coroutine_handle<promise_type>;

        Generator(handle_type h) : handle(h) {}
        ~Generator()
        {
            if (handle)
                handle.destroy();
        }

        Generator(const Generator&) = delete;
        Generator& operator=(const Generator&) = delete;

        Generator(Generator&& other) noexcept : handle(other.handle) { other.handle = nullptr; }

        Generator& operator=(Generator&& other) noexcept
        {
            if (this != &other)
            {
                if (handle)
                    handle.destroy();
                handle = other.handle;
                other.handle = nullptr;
            }
            return *this;
        }

        class Iterator
        {
        public:
            void operator++() { generator.handle.resume(); }

            template <class U = T>
                requires std::is_reference_v<U>
            U operator*() const
            {
                return *generator.handle.promise().current_value;
            }
            template <class U = T>
                requires(!std::is_reference_v<U>)
            U operator*() const
            {
                return generator.handle.promise().current_value;
            }
            bool operator==(std::default_sentinel_t) const { return !generator.handle || generator.handle.done(); }

            Iterator(Generator<T>& generator) : generator(generator) {}

        private:
            Generator<T>& generator;
        };

        Iterator begin()
        {
            if (handle)
            {
                handle.resume();
                if (handle.done())
                    return {*this};
            }
            return {*this};
        }

        std::default_sentinel_t end() { return {}; }

        struct promise_type
        {
            using ValueType = std::conditional_t<std::is_reference<T>::value, std::remove_reference_t<T>*, T>;
            ValueType current_value;
            std::exception_ptr exception;
            promise_type() : current_value(), exception() {}
            Generator get_return_object() { return Generator{handle_type::from_promise(*this)}; }

            std::suspend_always initial_suspend() { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }

            std::suspend_always yield_value(T&& value)
            {
                if constexpr (std::is_reference<T>::value)
                {
                    current_value = &value; // Оборачиваем ссылку
                }
                else
                {
                    current_value = value;
                }
                return {};
            }
            T await_resume()
            {
                if (exception)
                {
                    std::rethrow_exception(exception);
                }

                if constexpr (std::is_reference<T>::value)
                {
                    return *current_value; // Извлекаем ссылку
                }
                else
                {
                    return current_value;
                }
            }

            void unhandled_exception() { exception = std::current_exception(); }

            void return_void() {}

            void rethrow_if_exception()
            {
                if (exception)
                {
                    std::rethrow_exception(exception);
                }
            }
        };

    private:
        handle_type handle;
    };
} // namespace BeeEngine