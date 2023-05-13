//
// Created by alexl on 14.05.2023.
//

#pragma once

#include <unordered_map>
#include "Core/TypeDefines.h"

namespace BeeEngine
{
    class ObjectPool
    {
    public:
        ObjectPool(size_t size);
        ~ObjectPool();

        void* Allocate(size_t size);
        void Free(void* ptr);
        void Clear();
    private:
        char* m_Pool;
        size_t m_PoolSize;
        size_t m_UsedSize;
        std::unordered_map<void*, size_t> m_Allocations;
    };
}
