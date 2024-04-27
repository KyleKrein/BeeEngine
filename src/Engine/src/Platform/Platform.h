//
// Created by Aleksandr on 13.03.2024.
//

#pragma once
#include <thread>

#include "Core/Color4.h"
#include "Core/Numbers.h"
namespace BeeEngine
{
    void ThreadSetAffinity(std::thread& thread, uint32_t core);

    void* PlatformMemoryAllocate(size_t size, size_t alignment);
    void PlatformMemoryFree(void* ptr, size_t size, size_t alignment);
    void* PlatformMemoryZero(void* ptr, size_t size);
    void* PlatformMemoryCopy(void* dest, const void* src, size_t size);
    void* PlatformMemorySet(void* dest, int32_t value, size_t size);

    //void PlatformConsolePrint(const char* message, Color4 foreground, Color4 background);
    // Returns the absolute time in seconds
    float64_t PlatformGetAbsoluteTime();
}