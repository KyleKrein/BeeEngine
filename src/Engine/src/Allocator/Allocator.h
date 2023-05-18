//
// Created by alexl on 13.05.2023.
//

#pragma once


//#include "Debug/MemoryProfiler.h"
#include <iostream>
//#include "Core/TypeDefines.h"
//#include "Core/Logging/Log.h"
/*
#ifdef WIN32 // если находимся на платформе Windows
#include <Windows.h>

// получаем адрес оригинальной функции malloc
inline void* (*original_malloc)(size_t) = reinterpret_cast<void*(*)(size_t)>(GetProcAddress(LoadLibrary("msvcrt.dll"), "malloc"));
inline void (*original_free)(void *) = (void (*)(void *)) GetProcAddress(LoadLibrary("msvcrt.dll"), "free");
#else // если находимся на платформе UNIX
#include <dlfcn.h>

// получаем адрес оригинальной функции malloc
inline void* (*original_malloc)(size_t) = reinterpret_cast<void*(*)(size_t)>(dlsym(RTLD_NEXT, "malloc"));
inline void (*original_free)(void *) = (void (*)(void *)) dlsym(dlopen("libc.so.6", RTLD_LAZY), "free");

#endif

// переопределяем функцию malloc
inline void* malloc(size_t size)
{
    printf_s("Address of pointer: %p\n", (void*)original_malloc);
    // вызываем оригинальную функцию malloc
    void* ptr = original_malloc(size);

    // здесь можно добавить логирование или другую логику для отслеживания выделений памяти
    //BeeEngine::MemoryProfiler::Allocate(ptr, size);

    return ptr;
}
inline void free(void* ptr)
{
    //BeeEngine::MemoryProfiler::Free(ptr);
    // вызываем оригинальную функцию free
    original_free(ptr);

    // здесь можно добавить логирование или другую логику для отслеживания освобождений памяти

}
*/
#if 0
typedef struct {
    void* ptr;
    size_t size;
    char* file;
    int line;
} mem_block;
const int MAX_BLOCKS = 1000;
inline mem_block mem_blocks[MAX_BLOCKS];
inline int num_blocks = 0;

inline void print_unfreed_mem() {
    if (num_blocks == 0) {
        printf("No unfreed memory blocks\n");
    } else {
        printf("Unfreed memory blocks:\n");
        for (int i = 0; i < num_blocks; i++) {
            printf("%p, %zu bytes, allocated at %s:%d\n",
                   mem_blocks[i].ptr,
                   mem_blocks[i].size,
                   mem_blocks[i].file,
                   mem_blocks[i].line);
        }
    }
}

inline void* bee_malloc(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    //printf("malloc called from %s:%d, size = %zu, ptr = %p\n", file, line, size, ptr);

    mem_blocks[num_blocks].ptr = ptr;
    mem_blocks[num_blocks].size = size;
    mem_blocks[num_blocks].file = (char*)file;
    mem_blocks[num_blocks++].line = line;

    BeeEngine::MemoryProfiler::Allocate(size);

    return ptr;
}
inline void bee_free(void* ptr, const char* file, int line) {
    //printf("free called from %s:%d, ptr = %p\n", file, line, ptr);

    for (int i = 0; i < num_blocks; i++) {
        if (mem_blocks[i].ptr == ptr) {
            free(ptr);
            BeeEngine::MemoryProfiler::Free(mem_blocks[i].size);
            mem_blocks[i] = mem_blocks[num_blocks-1];
            num_blocks--;
            return;
        }
    }


    free(ptr);
}
#define xmalloc(size) bee_malloc(size, __FILE__, __LINE__)
#define xfree(ptr) bee_free(ptr, __FILE__, __LINE__)

#endif

#define USE_CUSTOM_ALLOCATOR 1

#if USE_CUSTOM_ALLOCATOR
#include "Core/Memory/GeneralPurposeAllocator.h"
#define allocate_memory(size) GeneralPurposeAllocator::Allocate(size, 8)
#define free_memory(ptr) GeneralPurposeAllocator::Free(ptr)
#else
#define allocate_memory(size) malloc(size)
#define free_memory(ptr) free(ptr)
#endif


inline void* operator new(size_t size)
{
    void* ptr = allocate_memory(size);
    if(!ptr)
    {
        throw std::bad_alloc();
    }

    //BeeEngine::MemoryProfiler::Allocate(size);

    //std::cout << "Allocated memory: " << size << " bytes" << std::endl;

    return ptr;
}

inline void operator delete(void* ptr, size_t size) noexcept
{
    if(ptr == nullptr)
    {
        return;
    }

    //BeeEngine::MemoryProfiler::Free(size);

    //std::cout << "Freed memory: " << size << " bytes" << std::endl;

    free_memory(ptr);
}

inline void operator delete(void* ptr) noexcept
{
    if(ptr == nullptr)
    {
        return;
    }

    //BeeEngine::MemoryProfiler::Free(size);

    //std::cout << "Freed memory: " << size << " bytes" << std::endl;

    free_memory(ptr);
}

inline void* operator new[](size_t size)
{
    void* ptr = allocate_memory(size);
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

    free_memory(ptr);
}

namespace BeeEngine
{


}

