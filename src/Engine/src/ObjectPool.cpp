//
// Created by alexl on 14.05.2023.
//

#include "ObjectPool.h"


namespace BeeEngine
{

    ObjectPool::ObjectPool(size_t size)
        : m_Pool(new char[size]), m_PoolSize(size), m_UsedSize(0)
    {

    }

    ObjectPool::~ObjectPool()
    {
        Clear();
        delete[] m_Pool;
    }

    void *ObjectPool::Allocate(size_t size)
    {
        void* ptr;
        if (m_UsedSize + size <= m_PoolSize)
        {
            ptr = m_Pool + m_UsedSize;
            m_UsedSize += size;
        } else
        {
            ptr = ::operator new(size);
        }
        m_Allocations[ptr] = size;
        return ptr;
    }

    void ObjectPool::Free(void *ptr)
    {

    }

    void ObjectPool::Clear()
    {
        for(auto& [ptr, size] : m_Allocations)
        {
            if(ptr >= m_Pool && ptr < m_Pool + m_PoolSize)
            {
                m_UsedSize -= size;
            } else
            {
                ::operator delete(ptr, size);
            }
        }
        m_Allocations.clear();
    }
}
