//
// Created by alexl on 09.11.2023.
//
#pragma once
#include <version>
#if defined(__cpp_lib_stacktrace) && __cpp_lib_stacktrace >= 202011L
#include <stacktrace>
#endif
#include <vector>
#include "source_location.h"
#include "Core/String.h"
#include <sstream>
namespace BeeEngine
{
    class StackTrace
    {
    public:
        struct Entry
        {
            const String Description;
            const String FileName;
            const size_t LineNumber;
            Entry(String&& description, String&& fileName, size_t lineNumber) noexcept
                : Description(std::move(description)), FileName(std::move(fileName)), LineNumber(lineNumber) {}
            Entry(const String& description, const String& fileName, size_t lineNumber) noexcept
                : Description(description), FileName(fileName), LineNumber(lineNumber) {}
        };
#if defined(__cpp_lib_stacktrace) && __cpp_lib_stacktrace >= 202011L
        StackTrace(const std::stacktrace& stacktrace = std::stacktrace::current());
#elif defined(MACOS)
        StackTrace();
#else
        StackTrace(std::source_location location = std::source_location::current())
        {
            m_Entries.emplace_back(location.function_name(), location.file_name(), location.line());
        }
#endif
        [[nodiscard]] String ToString() const;
        auto begin() noexcept
        {
            return m_Entries.begin();
        }
        auto end() noexcept
        {
            return m_Entries.end();
        }
        auto cbegin() const noexcept
        {
            return m_Entries.cbegin();
        }
        auto cend() const noexcept
        {
            return m_Entries.cend();
        }
        auto rbegin() noexcept
        {
            return m_Entries.rbegin();
        }
        auto rend() noexcept
        {
            return m_Entries.rend();
        }
        size_t size() const noexcept
        {
            return m_Entries.size();
        }
        bool empty() const noexcept
        {
            return m_Entries.empty();
        }
    private:
        std::vector<Entry> m_Entries;
    };
} // BeeEngine
