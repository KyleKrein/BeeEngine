//
// Created by alexl on 13.05.2023.
//

#pragma once

#include "Core/TypeDefines.h"
namespace BeeEngine
{
    class MemoryProfiler
    {
    public:
        static void Allocate(size_t size);

        static void Free(size_t size);
        static size_t GetAllocatedSize();
    };
}
