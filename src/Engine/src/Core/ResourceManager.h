//
// Created by alexl on 14.05.2023.
//

#pragma once
#include "TypeDefines.h"
#include "algorithm"

namespace BeeEngine
{
    class ResourceManager
    {
    public:
        inline static String ProcessFilePath(const String &filepath)
        {
            String result = filepath;
            std::replace(result.begin(), result.end(), '\\', '/');
            return result;
        }
    };
}
