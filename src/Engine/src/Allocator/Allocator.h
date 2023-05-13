//
// Created by alexl on 13.05.2023.
//

#pragma once


#include "Debug/MemoryProfiler.h"
#include <iostream>
#include "Core/TypeDefines.h"
#include "Core/Logging/Log.h"


inline void* operator new(size_t size)
{
    void* ptr = malloc(size);
    if(!ptr)
    {
        throw std::bad_alloc();
    }

    BeeEngine::MemoryProfiler::Allocate(size);

    //std::cout << "Allocated memory: " << size << " bytes" << std::endl;

    return ptr;
}

inline void operator delete(void* ptr, size_t size) noexcept
{
    if(ptr == nullptr)
    {
        return;
    }

    BeeEngine::MemoryProfiler::Free(size);

    //std::cout << "Freed memory: " << size << " bytes" << std::endl;

    free(ptr);
}

inline void* operator new[](size_t size)
{
    void* ptr = malloc(size);
    if (!ptr) {
        throw std::bad_alloc();
    }

    BeeEngine::MemoryProfiler::Allocate(size);

    //std::cout << "Allocated memory: " << size << " bytes" << std::endl;

    return ptr;
}

inline void operator delete[](void* ptr, size_t size) noexcept
{
    if(ptr == nullptr)
    {
        return;
    }

    BeeEngine::MemoryProfiler::Free(size);

    //std::cout << "Freed memory: " << size << " bytes" << std::endl;

    free(ptr);
}

