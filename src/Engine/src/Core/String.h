//
// Created by alexl on 09.09.2023.
//

#pragma once
#include <string>
#include <filesystem>
#define UTF_CPP_CPLUSPLUS 201703L
#include "utf8.h"

namespace BeeEngine
{
    using String = std::string;
    using UTF8String = String;
    using UTF16String = std::u16string;

    bool IsValidString(const UTF8String& string);
    UTF16String ConvertUTF8ToUTF16(const UTF8String& string);
    UTF8String ConvertUTF16ToUTF8(const UTF16String& string);
    template<typename iterator, typename endIterator>
    char32_t GetNextUTF8Char(iterator& it, endIterator end)
    {
        char32_t codepoint = utf8::next(it, end);
        return codepoint;
    }


    constexpr std::size_t constexpr_strlen(const char* s)
    {
        return std::char_traits<char>::length(s);
        // or
        return std::string::traits_type::length(s);
    }

    constexpr std::size_t constexpr_wcslen(const wchar_t* s)
    {
        return std::char_traits<wchar_t>::length(s);
        // or
        return std::wstring::traits_type::length(s);
    }
}