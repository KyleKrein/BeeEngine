//
// Created by alexl on 17.09.2023.
//

#pragma once
#include <string>
#include "Core/String.h"

namespace BeeEngine::Internal
{
    //std::wstring GetWString(const char* str);
    std::wstring WStringFromUTF8(const UTF8String &utf8string);
    UTF8String WStringToUTF8(const std::wstring& widestring);
}
