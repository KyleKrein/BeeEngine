//
// Created by alexl on 13.05.2023.
//

#include "MemoryProfiler.h"


namespace BeeEngine
{
        size_t s_AllocatedSize = 0;

        void MemoryProfiler::Allocate(size_t size)
        {
            s_AllocatedSize += size;
        }

        void MemoryProfiler::Free(size_t size)
        {
            s_AllocatedSize -= size;
        }

        size_t MemoryProfiler::GetAllocatedSize()
        {
            return s_AllocatedSize;
        }
}
