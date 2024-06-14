//
// Created by Александр Лебедев on 21.08.2023.
//

#pragma once
#include <string>
#include <utility>
#include <variant>
#include <version>
#ifdef __cpp_lib_source_location
#include "source_location"
#else
#include "source_location.h"
#include "spdlog/fmt/bundled/format.h"

#endif

namespace BeeEngine
{
    class Error
    {
    public:
        Error(std::source_location location = std::source_location::current())
        {
            m_Message = "Error at " + std::string(location.file_name()) + ":" + std::to_string(location.line()) +
                        " in function " + std::string(location.function_name());
        }

        Error(std::string_view format, std::source_location location = std::source_location::current())
        {
            m_Message = "Error at " + std::string(location.file_name()) + ":" + std::to_string(location.line()) +
                        " in function " + std::string(location.function_name()) + " Message: ";
        }

        Error(Error&& other) = default;
        Error& operator=(Error&& other) = default;
        Error(const Error& other) = default;
        Error& operator=(const Error& other) = default;
        ~Error() = default;

        const std::string& GetMessage() const { return m_Message; }
        operator const std::string&() const { return m_Message; }

    private:
        std::string m_Message;
    };

    template <typename E>
    struct Unexpected
    {
        E Error;
    };

#define ErrorHappened(...)                                                                                             \
    Unexpected<Error>                                                                                                  \
    {                                                                                                                  \
        Error                                                                                                          \
        {                                                                                                              \
            fmt::format(__VA_ARGS__)                                                                                   \
        }                                                                                                              \
    }

    template <typename T, typename E>
    class Expected
    {
    public:
        Expected(const T& value) : m_Data({value}), m_HasValue(true) {}
        Expected(T&& value) : m_Data({std::move(value)}), m_HasValue(true) {}
        Expected(const Unexpected<E>& error) : m_Data({error.Error}), m_HasValue(false) {}
        Expected(Unexpected<E>&& error) : m_Data({std::move(error.Error)}), m_HasValue(false) {}

        Expected(const Expected& other) = default;
        Expected(Expected&& other) noexcept = default;
        Expected& operator=(const Expected& other) = default;
        Expected& operator=(Expected&& other) noexcept = default;

        bool HasValue() const { return m_HasValue; }
        operator bool() const { return m_HasValue; }

        T Value()
        {
            if (!m_HasValue)
                throw std::get<E>(m_Data);
            return std::move(std::get<T>(m_Data));
        }
        E Error()
        {
            // if(m_HasValue) throw;
            return std::move(std::get<E>(m_Data));
        }

    private:
        std::variant<T, E> m_Data;
        bool m_HasValue;
    };
    template <typename E>
    class Expected<void, E>
    {
    public:
        Expected() : m_HasValue(true) {}
        Expected(const Unexpected<E>& error) : m_Data({error.Error}), m_HasValue(false) {}
        Expected(Unexpected<E>&& error) : m_Data({std::move(error.Error)}), m_HasValue(false) {}

        Expected(const Expected& other) = default;
        Expected(Expected&& other) noexcept = default;
        Expected& operator=(const Expected& other) = default;
        Expected& operator=(Expected&& other) noexcept = default;

        bool HasValue() const { return m_HasValue; }
        operator bool() const { return m_HasValue; }
        const E&& Error() const
        {
            // if(m_HasValue) throw;
            return std::move(m_Data);
        }

    private:
        E m_Data;
        bool m_HasValue;
    };
} // namespace BeeEngine