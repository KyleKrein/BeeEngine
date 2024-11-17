//
// Created by alexl on 10.11.2023.
//

#pragma once
#include <Core/String.h>

namespace BeeEngine
{
    String RunCommand(const String& command);

#if defined(IOS) || defined(ANDROID)
    void RunCommand(const String& command)
    {
        throw std::runtime_error("RunCommand is not implemented on mobile platforms!");
    }
#endif
} // namespace BeeEngine