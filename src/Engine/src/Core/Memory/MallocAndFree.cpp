//
// Created by Александр Лебедев on 16.05.2023.
//

#include <cstdlib>
#include "MallocAndFree.h"
#include "Debug/Instrumentor.h"

#define BEE_MEMORY_ALIGNMENT 8
#define USE_CUSTOM_ALLOCATION 0


//4 байта unsigned int - размер блока
//1 байт - bool false/true - занят/свободен
//4 байта unsigned int - размер блока до этого

void* bee_malloc(unsigned long long size)
{
#if USE_CUSTOM_ALLOCATION
    void* ptr = malloc(size + MEMORY_ALIGNMENT - 1 + sizeof(void*));
    if (!ptr) {
        return nullptr;
    }
    void* aligned_ptr = (void*)(((uintptr_t)(ptr) + MEMORY_ALIGNMENT) & ~(MEMORY_ALIGNMENT));
    ((void**)(aligned_ptr))[-1] = ptr;
    return aligned_ptr;
#else
    return malloc(size);
#endif
}

void bee_free(void* ptr)
{
#if USE_CUSTOM_ALLOCATION
    if (!ptr) {
        return;
    }
    free(((void**)(ptr))[-1]);
#else
    free(ptr);
#endif
}
