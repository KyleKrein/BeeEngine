//
// Created by alexl on 08.10.2023.
//

#pragma once
#include "String.h"
#include "ToString.h"

namespace BeeEngine
{
    namespace Internal
    {
        UTF8String CustomFormat_Impl(const UTF8String& format_str, const std::vector<UTF8String>& args);
    }
    template <typename... Args>
    UTF8String FormatString(const UTF8String& format, Args&&... args)
    {
        auto asStrings = std::vector<UTF8String>{BeeEngine::ToString(std::forward<Args>(args))...};

        return Internal::CustomFormat_Impl(format, asStrings);
    }
} // namespace BeeEngine