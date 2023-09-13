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
    using Path = std::filesystem::path;

    bool IsValidString(const UTF8String& string);
    UTF16String ConvertUTF8ToUTF16(const UTF8String& string);
    UTF8String ConvertUTF16ToUTF8(const UTF16String& string);
    template<typename iterator, typename endIterator>
    char32_t GetNextUTF8Char(iterator& it, endIterator end)
    {
        char32_t codepoint = utf8::next(it, end);
        return codepoint;
    }
}