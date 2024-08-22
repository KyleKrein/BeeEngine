#pragma once
#include "BufferLayout.h"

namespace BeeEngine
{
    struct BufferLayoutSerializer
    {
        static String Serialize(const BufferLayout& layout);
        static BufferLayout Deserialize(const String& str);
    };
} // namespace BeeEngine