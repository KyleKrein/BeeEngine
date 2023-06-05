//
// Created by alexl on 05.06.2023.
//

#pragma once

#include <string>
#include <optional>

namespace BeeEngine
{
    class FileDialogs
    {
    public:
        static std::optional<std::string> OpenFile(const char* filter);
        static std::optional<std::string> SaveFile(const char* filter);
    };
}