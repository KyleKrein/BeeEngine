//
// Created by alexl on 14.05.2023.
//

#pragma once

#include <gsl/gsl>
#include "TypeDefines.h"
#include "algorithm"
#include "Debug/Instrumentor.h"

namespace BeeEngine
{
    class ResourceManager
    {
    public:
        inline static String ProcessFilePath(std::string_view filepath)
        {
            BEE_PROFILE_FUNCTION();
            String result = String(filepath);
            std::replace(result.begin(), result.end(), '\\', '/');
            return result;
        }
        inline static String GetNameFromFilePath(std::string_view filepath)
        {
            BEE_PROFILE_FUNCTION();
            String result = ResourceManager::ProcessFilePath(filepath);
            int lastDot = result.find_last_of('.');
            int lastSlash = result.find_last_of('/') + 1;
            int count = lastDot == -1? result.size() - lastSlash: lastDot - lastSlash;
            return String(result.substr(lastSlash, count));
        }
    };
}
