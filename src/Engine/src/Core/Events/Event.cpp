//
// Created by alexl on 13.05.2023.
//
#include "Event.h"

namespace BeeEngine
{
    size_t Event::s_PoolSize = 1024; // 1 KB for now
    char* Event::s_Pool = new char[Event::s_PoolSize];
    size_t Event::s_UsedSize = 0;

    void* Event::operator new(size_t size)
    {
        BeeCoreTrace("Allocating {0} bytes. PoolSize: {1}. Used: {2}", size, s_PoolSize, s_UsedSize);
        if(s_UsedSize + size > s_PoolSize)
        {
            return ::operator new(size);
        }
        void* ptr = s_Pool + s_UsedSize;
        s_UsedSize += size;
        return ptr;
    }

    void Event::operator delete(void *ptr, size_t size) noexcept
    {
        BeeCoreTrace("Deallocating {0} bytes. PoolSize: {1}. Used: {2}", size, s_PoolSize, s_UsedSize);
        if (ptr < s_Pool || ptr >= s_Pool + s_PoolSize)
        {
            ::operator delete(ptr);
            return;
        }
        s_UsedSize -= size;
    }

    void* Event::operator new[](size_t size)
    {
        BeeCoreTrace("Allocating {0} bytes. PoolSize: {1}. Used: {2}", size, s_PoolSize, s_UsedSize);
        if(s_UsedSize + size > s_PoolSize)
        {
            return ::operator new(size);
        }
        void* ptr = s_Pool + s_UsedSize;
        s_UsedSize += size;
        return ptr;
    }

    void Event::operator delete[](void *ptr, size_t size) noexcept
    {
        BeeCoreTrace("Deallocating {0} bytes. PoolSize: {1}. Used: {2}", size, s_PoolSize, s_UsedSize);
        if (ptr < s_Pool || ptr >= s_Pool + s_PoolSize)
        {
            ::operator delete(ptr);
            return;
        }
        s_UsedSize -= size;
    }
}