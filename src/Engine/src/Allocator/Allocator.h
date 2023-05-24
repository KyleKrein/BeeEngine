//
// Created by alexl on 13.05.2023.
//

#pragma once


#include <iostream>
#include "cstddef"


#define USE_CUSTOM_ALLOCATOR 1

#if USE_CUSTOM_ALLOCATOR
#ifndef BEE_TEST_MODE
#include "Core/Memory/GeneralPurposeAllocator.h"
#include "Debug/Instrumentor.h"

#define bee_allocate_memory(size) BeeEngine::Internal::GeneralPurposeAllocator::Allocate(size, 16)
#define bee_allocate_aligned_memory(size, alignment) BeeEngine::Internal::GeneralPurposeAllocator::Allocate(size, alignment)
#define bee_free_memory(ptr) BeeEngine::Internal::GeneralPurposeAllocator::Free(ptr)
#else
#define bee_allocate_memory(size) malloc(size)
#define bee_allocate_aligned_memory(size, alignment) malloc(size)
#define bee_free_memory(ptr) free(ptr)
#endif
#else
#define bee_allocate_memory(size) malloc(size)
#define bee_allocate_aligned_memory(size, alignment) malloc(size)
#define bee_free_memory(ptr) free(ptr)
#endif

#ifndef BEE_TEST_MODE1

#if USE_CUSTOM_ALLOCATOR
inline void* operator new(std::size_t size, std::align_val_t alignment)
{
    std::cout << "Aligned memory allocator was used" << std::endl;
    void* ptr = bee_allocate_aligned_memory(size, (size_t)alignment);
    if(!ptr)
    {
        throw std::bad_alloc();
    }

    return ptr;
}
#endif

inline void* operator new(std::size_t size)
{
    void* ptr = bee_allocate_aligned_memory(size, alignof(std::max_align_t));
    if(!ptr)
    {
        throw std::bad_alloc();
    }

    //BeeEngine::MemoryProfiler::Allocate(size);

    //std::cout << "Allocated memory: " << size << " bytes" << std::endl;

    return ptr;
}

inline void* operator new(std::size_t size, const std::nothrow_t&) noexcept
{
    void* ptr = bee_allocate_aligned_memory(size, alignof(std::max_align_t));
    if(!ptr)
    {
        return nullptr;
    }

    return ptr;
}

inline void* operator new(std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
    void* ptr = bee_allocate_aligned_memory(size, (size_t)alignment);
    if(!ptr)
    {
        return nullptr;
    }

    return ptr;
}

inline void operator delete(void* ptr, const std::nothrow_t&) noexcept
{
    if(ptr == nullptr)
    {
        return;
    }

    bee_free_memory(ptr);
}

inline void operator delete(void* ptr, std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
    if(ptr == nullptr)
    {
        return;
    }

    bee_free_memory(ptr);
}


inline void operator delete(void* ptr, size_t size) noexcept
{
    if(ptr == nullptr)
    {
        return;
    }

    //BeeEngine::MemoryProfiler::Free(size);

    //std::cout << "Freed memory: " << size << " bytes" << std::endl;

    bee_free_memory(ptr);
}

inline void operator delete (void* ptr, std::align_val_t alignment) noexcept
{
    if(ptr == nullptr)
    {
        return;
    }
    bee_free_memory(ptr);
}

inline void operator delete(void* ptr) noexcept
{
    if(ptr == nullptr)
    {
        return;
    }

    //BeeEngine::MemoryProfiler::Free(size);

    //std::cout << "Freed memory: " << size << " bytes" << std::endl;

    bee_free_memory(ptr);
}

inline void* operator new[](size_t size)
{
    void* ptr = bee_allocate_memory(size);
    if (!ptr) {
        throw std::bad_alloc();
    }

    //BeeEngine::MemoryProfiler::Allocate(size);

    return ptr;
}

inline void operator delete[](void* ptr, size_t size) noexcept
{
    if(ptr == nullptr)
    {
        return;
    }

    //BeeEngine::MemoryProfiler::Free(size);

    //std::cout << "Freed memory: " << size << " bytes" << std::endl;

    bee_free_memory(ptr);
}
inline void* operator new[](std::size_t size, std::align_val_t alignment)
{
    void* ptr = bee_allocate_aligned_memory(size, (size_t)alignment);
    if (!ptr) {
        throw std::bad_alloc();
    }

    return ptr;
}
inline void operator delete[] (void* ptr, std::align_val_t alignment) noexcept
{
    if(ptr == nullptr)
    {
        return;
    }
    bee_free_memory(ptr);
}
#endif

namespace BeeEngine
{


}

