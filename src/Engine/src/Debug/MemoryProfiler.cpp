//
// Created by alexl on 13.05.2023.
//

#include "MemoryProfiler.h"


namespace BeeEngine
{
        size_t s_AllocatedSize = 0;
        std::unordered_map<void*, size_t> MemoryProfiler::m_AllocatedMemory;

        void MemoryProfiler::Allocate(size_t size)
        {
            s_AllocatedSize += size;
        }

        void MemoryProfiler::Allocate(void* ptr, size_t size)
        {
            m_AllocatedMemory[ptr] = size;
            s_AllocatedSize += size;
        }

        void MemoryProfiler::Free(size_t size)
        {
            s_AllocatedSize -= size;
        }

        void MemoryProfiler::Free(void* ptr)
        {
            s_AllocatedSize -= m_AllocatedMemory[ptr];
            m_AllocatedMemory.erase(ptr);
        }

        size_t MemoryProfiler::GetAllocatedSize()
        {
            return s_AllocatedSize;
        }
}
