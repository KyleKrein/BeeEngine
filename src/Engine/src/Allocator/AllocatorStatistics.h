//
// Created by alexl on 25.05.2023.
//

#pragma once
#include <atomic>
#include <cstddef>
#include <iostream>

namespace BeeEngine::Internal
{
    struct AllocatorStatistics
    {
        std::atomic<size_t> totalAllocatedMemory;
        std::atomic<size_t> allocatedMemory;
        std::atomic<size_t> gcHeapSize;
        std::atomic<size_t> gcUsedMemory;
        std::atomic<size_t> gcGenerations;
        std::atomic<size_t> totalFreedMemory;

        inline static AllocatorStatistics& GetStatistics()
        {
            static AllocatorStatistics s_Statistics;
            return s_Statistics;
        }
    };
} // namespace BeeEngine::Internal