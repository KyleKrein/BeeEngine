//
// Created by alexl on 17.09.2023.
//

#include "WindowsString.h"
#include <codecvt>

namespace BeeEngine::Internal
{
    // convert char* string to wstring WARNING: NOT TESTED
    std::wstring GetWString(const char* str)
    {
        const size_t size = strlen(str) + 1;
        std::wstring wstr;
        wstr.resize(size);
        mbstowcs(&wstr[0], str, size);
        return wstr;
    }
    // convert UTF-8 string to wstring
    std::wstring WStringFromUTF8(const UTF8String& utf8string)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        return myconv.from_bytes(utf8string);
    }

    // convert wstring to UTF-8 string
    UTF8String WStringToUTF8(const std::wstring& widestring)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        return myconv.to_bytes(widestring);
    }
} // namespace BeeEngine::Internal
