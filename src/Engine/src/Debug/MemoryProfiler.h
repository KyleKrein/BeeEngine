//
// Created by alexl on 13.05.2023.
//

#pragma once

#include "Core/TypeDefines.h"
#include "unordered_map"
namespace BeeEngine
{
    class MemoryProfiler
    {
    public:
        static void Allocate(size_t size);
        static void Allocate(void* ptr, size_t size);

        static void Free(size_t size);
        static void Free(void* size);
        static size_t GetAllocatedSize();

    private:
        static std::unordered_map<void*, size_t> m_AllocatedMemory;
    };

} // namespace BeeEngine
