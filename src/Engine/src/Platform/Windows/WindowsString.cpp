//
// Created by alexl on 17.09.2023.
//

#include "WindowsString.h"
#include <windows.h>

namespace BeeEngine::Internal
{
    // convert UTF-8 string to wstring
    std::wstring WStringFromUTF8(const UTF8String& utf8string)
    {
        if (utf8string.empty())
        {
            return L"";
        }

        const auto size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8string.data(), (int)utf8string.size(), nullptr, 0);
        if (size_needed <= 0)
        {
            // TODO: decide whether to throw or return empty string or report error some other way
            throw std::runtime_error("MultiByteToWideChar() failed: " + std::to_string(size_needed));
        }

        std::wstring result(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, utf8string.data(), (int)utf8string.size(), result.data(), size_needed);
        return result;
    }

    // convert wstring to UTF-8 string
    UTF8String WStringToUTF8(const std::wstring& widestring)
    {
        if (widestring.empty())
        {
            return "";
        }

        const auto size_needed =
            WideCharToMultiByte(CP_UTF8, 0, widestring.data(), (int)widestring.size(), nullptr, 0, nullptr, nullptr);
        if (size_needed <= 0)
        {
            // TODO: decide whether to throw or return empty string or report error some other way
            throw std::runtime_error("WideCharToMultiByte() failed: " + std::to_string(size_needed));
        }

        UTF8String result(size_needed, 0);
        WideCharToMultiByte(
            CP_UTF8, 0, widestring.data(), (int)widestring.size(), result.data(), size_needed, nullptr, nullptr);
        return result;
    }
} // namespace BeeEngine::Internal
