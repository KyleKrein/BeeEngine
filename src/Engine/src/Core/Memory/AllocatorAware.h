//
// Created by Александр Лебедев on 23.05.2023.
//

#pragma once

#ifdef MACOS
#include <experimental/memory_resource>
#else
#include "memory_resource"
#endif
#if 0
namespace BeeEngine
{
    class AllocatorAware
    {
    public:
        AllocatorAware() = default;
        AllocatorAware(const AllocatorAware&) = default;
        AllocatorAware(AllocatorAware&&) = default;
        AllocatorAware& operator=(const AllocatorAware&) = default;
        AllocatorAware& operator=(AllocatorAware&&) = default;
        virtual ~AllocatorAware() = default;

        void* operator new(std::size_t size)
        {
            return m_Allocator.allocate(size);
        }

        void* operator new(std::size_t size, std::align_val_t alignment)
        {
            return m_Allocator.allocate(size, alignment);
        }

        void operator delete(void* ptr, std::size_t size)
        {
            m_Allocator.deallocate(static_cast<std::byte*>(ptr), size);
        }

        void operator delete(void* ptr, std::size_t size, std::align_val_t alignment)
        {
            m_Allocator.deallocate(static_cast<std::byte*>(ptr), size, alignment);
        }

        void operator delete(void* ptr)
        {
            m_Allocator.deallocate(static_cast<std::byte*>(ptr), sizeof(*this));
        }

        void operator delete(void* ptr, std::align_val_t alignment)
        {
            m_Allocator.deallocate(static_cast<std::byte*>(ptr), sizeof(*this), alignment);
        }
    private:
#ifdef MACOS
        std::experimental::pmr::polymorphic_allocator<std::byte> m_Allocator;
#else
        std::pmr::polymorphic_allocator<> m_Allocator;
#endif
    };
}
#endif
