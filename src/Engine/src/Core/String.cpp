//
// Created by alexl on 09.09.2023.
//

#include "String.h"
#include "Core/CodeSafety/Expects.h"
#define UTF_CPP_CPLUSPLUS 201703L
#include <utf8.h>

namespace BeeEngine
{

    bool IsValidString(const UTF8String &string)
    {
        return utf8::is_valid(string.begin(), string.end());
    }

    UTF16String ConvertUTF8ToUTF16(const UTF8String &string)
    {
        BeeExpects(IsValidString(string));
        return utf8::utf8to16(string);
    }

    UTF8String ConvertUTF16ToUTF8(const UTF16String &string)
    {
        UTF8String result;
        utf8::utf16to8(string.begin(), string.end(), std::back_inserter(result));
        BeeEnsures(IsValidString(result));
        return result;
    }
}
