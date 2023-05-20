//
// Created by alexl on 13.05.2023.
//
#include "Event.h"

namespace BeeEngine
{
    /*
    ObjectPool Event::s_EventPool(1024);

    void* Event::operator new(size_t size)
    {
        return s_EventPool.Allocate(size);
    }

    void Event::operator delete(void *ptr, size_t size) noexcept
    {
        s_EventPool.Free(ptr);
    }

    void* Event::operator new[](size_t size)
    {
        return s_EventPool.Allocate(size);
    }

    void Event::operator delete[](void *ptr, size_t size) noexcept
    {
        s_EventPool.Free(ptr);
    }
     */
}