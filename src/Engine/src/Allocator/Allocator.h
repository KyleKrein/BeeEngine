//
// Created by alexl on 13.05.2023.
//

#pragma once




#include "Core/TypeDefines.h"
#include "AllocatorStatistics.h"

namespace BeeEngine::Internal
{
    class MemoryTracker {
        struct AllocationRecord
        {
            void* Ptr = nullptr;
            std::size_t Size = 0;
        };
    public:
        // Функция для аллокации памяти
        static void alloc(void* ptr, std::size_t size) noexcept{
            ensureCapacity();
            // Поиск первой свободной ячейки
            for (std::size_t i = 0; i < capacity; ++i) {
                if (!records[i].Ptr) {
                    new (&records[i]) AllocationRecord{ptr, size};  // Placement new
                    BeeEnsures(records[i].Ptr == ptr);
                    BeeEnsures(records[i].Size == size);
                    ++recordCount;
                    break;
                }
            }
            AllocatorStatistics::GetStatistics().allocatedMemory += size;
            AllocatorStatistics::GetStatistics().totalAllocatedMemory += size;
        }

        // Функция для освобождения памяти
        static void free(void* ptr) {
            for (std::size_t i = 0; i < capacity; ++i) {
                if (records[i].Ptr == ptr) {
                    AllocatorStatistics::GetStatistics().allocatedMemory -= records[i].Size;
                    AllocatorStatistics::GetStatistics().totalAllocatedMemory -= records[i].Size;
                    records[i].~AllocationRecord();  // Вызов деструктора
                    --recordCount;
                    break;
                }
            }
            debug_break();
        }

    private:
        static AllocationRecord* records;
        static std::size_t recordCount;
        static std::size_t capacity;

        // Функция для увеличения размера массива records при необходимости
        static void ensureCapacity() noexcept {
            if(capacity == 0)
            {
                capacity = 100;
                records = (AllocationRecord*)malloc(capacity * sizeof(AllocationRecord));
                memset(records, 0, capacity * sizeof(AllocationRecord));
                //if (!records) throw std::bad_alloc();
                return;
            }
            if (recordCount == capacity) {
                std::size_t newCapacity = capacity * 2 + 1;
                AllocationRecord* newRecords = (AllocationRecord*)malloc(newCapacity * sizeof(AllocationRecord));
                memset(records, 0, capacity * sizeof(AllocationRecord));
                //if (!newRecords) throw std::bad_alloc();
                for (std::size_t i = 0; i < capacity; ++i) {
                    new (&newRecords[i]) AllocationRecord(std::move(records[i]));  // Placement new
                    records[i].~AllocationRecord();  // Вызов деструктора
                }
                free(records);  // Освобождение старого массива
                records = newRecords;
                capacity = newCapacity;
            }
        }
    };
}
#define USE_CUSTOM_ALLOCATOR 0

#if USE_CUSTOM_ALLOCATOR
#ifndef BEE_TEST_MODE
#include "GeneralPurposeAllocator.h"
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
    void* ptr = bee_allocate_aligned_memory(size, (size_t)alignment);
    if(!ptr)
    {
        throw std::bad_alloc();
    }

    return ptr;
}


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
#endif

inline void* operator new(std::size_t size, std::align_val_t alignment)
{
    void* ptr = malloc(size);
    if(!ptr)
    {
        throw std::bad_alloc();
    }
    BeeEngine::Internal::MemoryTracker::alloc(ptr, size);
    return ptr;
}


inline void* operator new(std::size_t size)
{
    void* ptr = malloc(size);
    if(!ptr)
    {
        throw std::bad_alloc();
    }
    BeeEngine::Internal::MemoryTracker::alloc(ptr, size);
    return ptr;
}

inline void* operator new(std::size_t size, const std::nothrow_t&) noexcept
{
    void* ptr = malloc(size);
    if(!ptr)
    {
        return nullptr;
    }
    BeeEngine::Internal::MemoryTracker::alloc(ptr, size);
    return ptr;
}

inline void* operator new(std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
    void* ptr = malloc(size);
    if(!ptr)
    {
        return nullptr;
    }
    BeeEngine::Internal::MemoryTracker::alloc(ptr, size);
    return ptr;
}

inline void operator delete(void* ptr, const std::nothrow_t&) noexcept
{
    if(ptr == nullptr)
    {
        return;
    }

    free(ptr);
    BeeEngine::Internal::MemoryTracker::free(ptr);
}

inline void operator delete(void* ptr, std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
    if(ptr == nullptr)
    {
        return;
    }

    free(ptr);
    BeeEngine::Internal::MemoryTracker::free(ptr);
}


inline void operator delete(void* ptr, size_t size) noexcept
{
    if(ptr == nullptr)
    {
        return;
    }

    free(ptr);
    BeeEngine::Internal::MemoryTracker::free(ptr);
}

inline void operator delete (void* ptr, std::align_val_t alignment) noexcept
{
    if(ptr == nullptr)
    {
        return;
    }
    free(ptr);
    BeeEngine::Internal::MemoryTracker::free(ptr);
}

inline void operator delete(void* ptr) noexcept
{
    if(ptr == nullptr)
    {
        return;
    }

    free(ptr);
    BeeEngine::Internal::MemoryTracker::free(ptr);
}

inline void* operator new[](size_t size)
{
    void* ptr = malloc(size);
    if (!ptr) {
        throw std::bad_alloc();
    }
    BeeEngine::Internal::MemoryTracker::alloc(ptr, size);
    return ptr;
}

inline void operator delete[](void* ptr, size_t size) noexcept
{
    if(ptr == nullptr)
    {
        return;
    }

    free(ptr);
    BeeEngine::Internal::MemoryTracker::free(ptr);
}
inline void* operator new[](std::size_t size, std::align_val_t alignment)
{
    void* ptr = malloc(size);
    if (!ptr) {
        throw std::bad_alloc();
    }
    BeeEngine::Internal::MemoryTracker::alloc(ptr, size);
    return ptr;
}
inline void operator delete[] (void* ptr, std::align_val_t alignment) noexcept
{
    if(ptr == nullptr)
    {
        return;
    }
    free(ptr);
    BeeEngine::Internal::MemoryTracker::free(ptr);
}

