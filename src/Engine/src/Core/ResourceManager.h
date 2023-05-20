//
// Created by alexl on 14.05.2023.
//

#pragma once
#include "TypeDefines.h"
#include "algorithm"
#include "Debug/Instrumentor.h"

namespace BeeEngine
{
    class ResourceManager
    {
    public:
        inline static String ProcessFilePath(const String &filepath)
        {
            BEE_PROFILE_FUNCTION();
            String result = filepath;
            std::replace(result.begin(), result.end(), '\\', '/');
            return result;
        }
    };
}
