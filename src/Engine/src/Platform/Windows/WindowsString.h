//
// Created by alexl on 17.09.2023.
//

#pragma once
#include "Core/String.h"
#include <string>

namespace BeeEngine::Internal
{
    // std::wstring GetWString(const char* str);
    std::wstring WStringFromUTF8(const UTF8String& utf8string);
    UTF8String WStringToUTF8(const std::wstring& widestring);
} // namespace BeeEngine::Internal
