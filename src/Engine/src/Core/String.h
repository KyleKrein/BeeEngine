//
// Created by alexl on 09.09.2023.
//

#pragma once
#include <SIMDString/SIMDString.h>
#include <cstddef>
#include <filesystem>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace BeeEngine
{
    using String = SIMDString<>;
    using UTF8String = String;
    using UTF16String = std::u16string;

    namespace StringLiterals
    {
        /**
         * @brief Operator, that converts any
         * C-string into UTF8String
         * @param arg C-String
         * @return UTF8String
         */
        inline UTF8String operator""_u8(const char* arg)
        {
            return {arg};
        }
    } // namespace StringLiterals
    /**
     * @brief Checks if argument
     * is a valid UTF8 string
     * @param string
     * @return true if string is valid
     * @return false if string is invalid
     */
    bool IsValidString(const UTF8String& string);
    UTF16String ConvertUTF8ToUTF16(const UTF8String& string);
    UTF8String ConvertUTF16ToUTF8(const UTF16String& string);

    /**
     * @brief a string_view like class
     * that allows to iterate on unicode
     * characters (read only)
     */
    class UTF8StringView
    {
    public:
        /**
         * @brief Construct a new UTF8StringView object
         *
         * @param string must be a string with lifetime, that
         * exceeds lifetime of UTF8StringView object.
         * String literals and temporary values are not allowed
         */
        UTF8StringView(const UTF8String& string);
        UTF8StringView(UTF8String&&) = delete;
        UTF8StringView(const char*) = delete;
        class iterator
        {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = char32_t;
            using difference_type = std::string::difference_type;
            using pointer = const char32_t*;
            using reference = const char32_t&;

            iterator(UTF8String::const_iterator it);
            iterator(const iterator& source);
            iterator& operator=(const iterator& rhs);
            ~iterator();

            iterator& operator++();
            iterator operator++(int);
            iterator& operator--();
            iterator operator--(int);
            bool operator==(const iterator& rhs) const;
            bool operator!=(const iterator& rhs) const;
            char32_t operator*() const;

        private:
            UTF8String::const_iterator m_StringIterator;
            mutable char32_t m_Codepoint = 0;
            mutable bool m_IsDirty = true;

            void UpdateCodepoint() const;
        };
        iterator begin() const { return {m_String->begin()}; }
        iterator end() const { return {m_String->end()}; }

    private:
        const UTF8String* m_String;
    };

    constexpr std::size_t constexpr_strlen(const char* s)
    {
#if defined(_MSC_VER)
        if constexpr (std::is_constant_evaluated())
#else
        if consteval
#endif
        {
            size_t len = 0;
            for (const char* str = s; *str; ++str)
            {
                ++len;
            }
            return len;
        }

        return ::strlen(s);
    }

    constexpr std::size_t constexpr_wcslen(const wchar_t* s)
    {
#if defined(_MSC_VER)
        if constexpr (std::is_constant_evaluated())
#else
        if consteval
#endif
        {
            size_t len = 0;
            for (const wchar_t* str = s; *str; ++str)
            {
                ++len;
            }
            return len;
        }

        return ::wcslen(s);
    }

    constexpr std::vector<std::string_view> SplitString(std::string_view str, std::string_view delimiters)
    {
        std::vector<std::string_view> output;
        size_t first = 0;

        while (first < str.size())
        {
            const auto second = str.find_first_of(delimiters, first);

            if (first != second)
                output.emplace_back(str.substr(first, second - first));

            if (second == std::string_view::npos)
                break;

            first = second + 1;
        }

        return output;
    }
    constexpr void ReplaceAllSubstrings(String& s, const String& search, const String& replace)
    {
        for (size_t pos = 0;; pos += replace.length())
        {
            // Locate the substring to replace
            pos = s.find(search, pos);
            if (pos == std::string::npos)
                break;
            // Replace by erasing and inserting
            s.erase(pos, search.length());
            s.insert(pos, replace);
        }
    }

    UTF8String ToUppercase(std::string_view string);
    UTF8String ToLowercase(std::string_view string);
    inline UTF8String ToUppercase(const String& string)
    {
        return ToUppercase(std::string_view{string.begin(), string.end()});
    }
    inline UTF8String ToLowercase(const String& string)
    {
        return ToLowercase(std::string_view{string.begin(), string.end()});
    }
} // namespace BeeEngine
using namespace BeeEngine::StringLiterals; // May be removed in future
#undef CONSTEXPR