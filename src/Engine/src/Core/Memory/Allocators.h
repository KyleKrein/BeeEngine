//
// Created by alexl on 17.11.2023.
//

#pragma once
#include "AllocatorPrimitives.h"
#include "Core/Logging/Log.h"
#undef max
namespace BeeEngine
{
    class NullAllocator
    {
    public:
        MemoryBlock Allocate(size_t size)
        {
            return MemoryBlock{nullptr, 0};
        }
        MemoryBlock AllocateAligned(size_t size, size_t alignment)
        {
            return MemoryBlock{nullptr, 0};
        }
        void Deallocate(MemoryBlock block)
        {
            if(!block.Ptr)
            {
                BeeCoreFatalError("Trying to deallocate nullptr");
            }
        }
        void DeallocateAligned(MemoryBlock block, size_t alignment)
        {
            if(!block.Ptr)
            {
                BeeCoreFatalError("Trying to deallocate nullptr");
            }
        }
        bool Owns(MemoryBlock block)
        {
            return false;
        }
        void DeallocateAll()
        {}
    };
    template<size_t stackSize>
    class StackAllocator
    {
    public:
        MemoryBlock Allocate(size_t size)
        {
            return AllocateAligned(size, 1);
        }
        MemoryBlock AllocateAligned(size_t size, size_t alignment)
        {
            size_t alignedOffset = AlignMemoryAddress(m_Offset + size, alignment);
            if(alignedOffset > stackSize)
            {
                BeeCoreError("StackAllocator overflow [Stack size: {0}, Used: {1}, Requested: {2}]", stackSize, m_Offset, size);
                return {nullptr, 0};
            }
            void* alignedPtr = AlignMemoryAddress(m_Data + m_Offset, alignment);
            m_Offset = alignedOffset;
            return {alignedPtr, size};
        }
        void Deallocate(MemoryBlock block)
        {
            if(!block.Ptr)
            {
                BeeCoreFatalError("Trying to deallocate nullptr");
            }
            if(block.Ptr != (m_Data + m_Offset - block.Size))
            {
                return;
            }
            m_Offset -= block.Size;
        }
        void DeallocateAligned(MemoryBlock block, size_t alignment)
        {
            if(!block.Ptr)
            {
                BeeCoreFatalError("Trying to deallocate nullptr");
            }
            if(block.Ptr != AlignMemoryAddress(m_Data + m_Offset - block.Size, alignment))
            {
                return;
            }
            m_Offset -= block.Size;
        }
        bool Owns(MemoryBlock block)
        {
            return block.Ptr >= m_Data && block.Ptr < m_Data + stackSize;
        }
        void DeallocateAll()
        {
            m_Offset = 0;
        }
    private:
        std::byte m_Data[stackSize];
        size_t m_Offset;
    };
    template<bool trackAllocations = false>
    class Mallocator
    {
    public:
        MemoryBlock Allocate(size_t size)
        {
            if constexpr (!trackAllocations)
            {
                return MemoryBlock{malloc(size), size};
            }
            else
            {
                void* ptr = malloc(size);
                m_Allocations.emplace_back(ptr, size);
                return MemoryBlock{ptr, size};
            }
        }
        void Deallocate(MemoryBlock block)
        {
            if(!block.Ptr)
            {
                BeeCoreFatalError("Trying to deallocate nullptr");
            }
            if constexpr (!trackAllocations)
            {
                free(block.Ptr);
            }
            else
            {
                auto it = std::find(m_Allocations.begin(), m_Allocations.end(), block);
                if(it != m_Allocations.end())
                {
                    m_Allocations.erase(it);
                }
                free(block.Ptr);
            }
        }
        MemoryBlock AllocateAligned(size_t size, size_t alignment)
        {
#if defined(_MSC_VER)
            void *ptr = _aligned_malloc(size, alignment);
#else
            void* ptr = aligned_alloc(alignment, size);
#endif
            if constexpr (trackAllocations)
            {
                m_Allocations.emplace_back(ptr, size);
            }
            return MemoryBlock{ptr, size};
        }
        void DeallocateAligned(MemoryBlock block, size_t alignment)
        {
            if(!block.Ptr)
            {
                BeeCoreFatalError("Trying to deallocate nullptr");
            }
#if defined(_MSC_VER)
            _aligned_free(block.Ptr);
#else
            free(block.Ptr);
#endif
            if constexpr (trackAllocations)
            {
                auto it = std::find(m_Allocations.begin(), m_Allocations.end(), block);
                if(it != m_Allocations.end())
                {
                    m_Allocations.erase(it);
                }
            }
        }
        bool Owns(MemoryBlock block)
        {
            if constexpr (trackAllocations)
            {
                return std::find(m_Allocations.begin(), m_Allocations.end(), block) != m_Allocations.end();
            }
            return true;
        }
        void DeallocateAll()
        {
            if constexpr (trackAllocations)
            {
                for(auto& block : m_Allocations)
                {
                    free(block.Ptr);
#if 0
                    if(block.Alignment == 0)
                    {
                        free(block.Ptr);
                    }
                    else
                    {
#if defined(_MSC_VER)
                        _aligned_free(block.Ptr);
#else
                        free(block.Ptr);
#endif
                    }
#endif

                }
                m_Allocations.clear();
            }
        }
    private:
        std::vector<MemoryBlock> m_Allocations;
    };

    template<MemoryAllocator Primary, MemoryAllocator Fallback>
    class FallbackAllocator: private Primary, private Fallback
    {
    public:
        MemoryBlock Allocate(size_t size)
        {
            MemoryBlock block = Primary::Allocate(size);
            if(!block.Ptr)
            {
                block = Fallback::Allocate(size);
            }
            return block;
        }
        void Deallocate(MemoryBlock block)
        {
            if(!block.Ptr)
            {
                BeeCoreFatalError("Trying to deallocate nullptr");
            }
            if(Primary::Owns(block))
            {
                Primary::Deallocate(block);
            }
            else
            {
                Fallback::Deallocate(block);
            }
        }
        MemoryBlock AllocateAligned(size_t size, size_t alignment)
        {
            MemoryBlock block = Primary::AllocateAligned(size, alignment);
            if(!block.Ptr)
            {
                block = Fallback::AllocateAligned(size, alignment);
            }
            return block;
        }
        void DeallocateAligned(MemoryBlock block, size_t alignment)
        {
            if(!block.Ptr)
            {
                BeeCoreFatalError("Trying to deallocate nullptr");
            }
            if(Primary::Owns(block))
            {
                Primary::DeallocateAligned(block, alignment);
            }
            else
            {
                Fallback::DeallocateAligned(block, alignment);
            }
        }
        bool Owns(MemoryBlock block)
        {
            return Primary::Owns(block) || Fallback::Owns(block);
        }
        void DeallocateAll()
        {
            Primary::DeallocateAll();
            Fallback::DeallocateAll();
        }
    };
    template<MemoryAllocator Allocator, size_t OnePageDefaultCapacity, size_t MaxPages>
    class FreeListAllocator: private Allocator
    {
    public:
        FreeListAllocator()
        : m_FreeList(AllocateNode())
        {}
        MemoryBlock Allocate(size_t size)
        {
            return AllocateAligned(size, 1);
        }
        void Deallocate(MemoryBlock block)
        {
            if(!block.Ptr)
            {
                BeeCoreFatalError("Trying to deallocate nullptr");
            }
            if(Owns(block))
            {
                Ptr<Node> prev = m_FreeList;
                while(prev)
                {
                    auto start = reinterpret_cast<uintptr_t>(prev->Memory.Ptr);
                    auto end = reinterpret_cast<uintptr_t>(prev->Memory.Ptr + prev->Offset);
                    auto blockPtr = reinterpret_cast<uintptr_t>(block.Ptr);
                    if(blockPtr >= start && blockPtr < end)
                    {
                        break;
                    }
                    prev = prev->Next;
                }
                prev->Offset -= block.Size;
            }
            else
            {
                Allocator::Deallocate(block);
            }
        }
        MemoryBlock AllocateAligned(size_t size, size_t alignment)
        {
            if(size > OnePageDefaultCapacity)
            {
                return Allocator::AllocateAligned(size, alignment);
            }
            Ptr<Node> prev = m_FreeList;
            while(size > prev->Memory.Size - prev->Offset - alignment)
            {
                if(!prev->Next)
                {
                    if(m_PageCount == MaxPages)
                    {
                        return Allocator::AllocateAligned(size, alignment);
                    }
                    prev->Next = AllocateNode();
                }
                prev = prev->Next;
            }
            void* ptr = AlignMemoryAddress(prev->Memory.Ptr + prev->Offset, alignment);
            prev->Offset += size + (ptr - prev->Memory.Ptr);
            return {ptr, size};
        }
        void DeallocateAligned(MemoryBlock block, size_t alignment)
        {
            if(Owns(block))
            {
                Ptr<Node> prev = m_FreeList;
                while(prev)
                {
                    auto start = reinterpret_cast<uintptr_t>(prev->Memory.Ptr);
                    auto end = reinterpret_cast<uintptr_t>(prev->Memory.Ptr + prev->Offset);
                    auto blockPtr = reinterpret_cast<uintptr_t>(block.Ptr);
                    if(blockPtr >= start && blockPtr < end)
                    {
                        break;
                    }
                    prev = prev->Next;
                }
                prev->Offset -= block.Size + (block.Ptr - prev->Memory.Ptr);
            }
            else
            {
                Allocator::DeallocateAligned(block, alignment);
            }
        }

        bool Owns(MemoryBlock block)
        {
            Ptr<Node> prev = m_FreeList;
            while(prev)
            {
                auto start = reinterpret_cast<uintptr_t>(prev->Memory.Ptr);
                auto end = reinterpret_cast<uintptr_t>(prev->Memory.Ptr + prev->Offset);
                auto blockPtr = reinterpret_cast<uintptr_t>(block.Ptr);
                if(blockPtr >= start && blockPtr < end)
                {
                    return true;
                }
                prev = prev->Next;
            }
            return false;
        }
        void DeallocateAll()
        {
            Ptr<Node> prev = m_FreeList;
            while(prev)
            {
                Ptr<Node> next = prev->Next;
                DeallocateNode(prev);
                prev = next;
            }
        }

    private:
        struct Node
        {
            Ptr<Node> Next;
            MemoryBlock Memory;
            size_t Offset;
        };
        Ptr<Node> m_FreeList;
        size_t m_PageCount;

        Ptr<Node> AllocateNode()
        {
            Ptr<Node> node = Allocator::Allocate(sizeof(Node));
            node->Next = nullptr;
            node->Memory = Allocator::Allocate(sizeof(OnePageDefaultCapacity));
            node->Offset = 0;
            m_PageCount++;
            return node;
        }
        void DeallocateNode(Ptr<Node> node)
        {
            Allocator::Deallocate(node->Memory);
            Allocator::Deallocate(node);
            m_PageCount--;
        }
    };
    template<size_t thresholdInBytes, MemoryAllocator FirstIfMore, MemoryAllocator SecondIfLessOrEqual>
    class SegregatorAllocator: private FirstIfMore, private SecondIfLessOrEqual
    {
    public:
        MemoryBlock Allocate(size_t size)
        {
            if(size > thresholdInBytes)
            {
                return FirstIfMore::Allocate(size);
            }
            return SecondIfLessOrEqual::Allocate(size);
        }
        void Deallocate(MemoryBlock block)
        {
            if(!block.Ptr)
            {
                BeeCoreFatalError("Trying to deallocate nullptr");
            }
            if(FirstIfMore::Owns(block))
            {
                FirstIfMore::Deallocate(block);
            }
            else
            {
                SecondIfLessOrEqual::Deallocate(block);
            }
        }
        MemoryBlock AllocateAligned(size_t size, size_t alignment)
        {
            if(size > thresholdInBytes)
            {
                return SecondIfLessOrEqual::AllocateAligned(size, alignment);
            }
            return FirstIfMore::AllocateAligned(size, alignment);
        }
        void DeallocateAligned(MemoryBlock block, size_t alignment)
        {
            if(!block.Ptr)
            {
                BeeCoreFatalError("Trying to deallocate nullptr");
            }
            if(FirstIfMore::Owns(block))
            {
                FirstIfMore::DeallocateAligned(block, alignment);
            }
            else
            {
                SecondIfLessOrEqual::DeallocateAligned(block, alignment);
            }
        }
        bool Owns(MemoryBlock block)
        {
            return FirstIfMore::Owns(block) || SecondIfLessOrEqual::Owns(block);
        }
        void DeallocateAll()
        {
            FirstIfMore::DeallocateAll();
            SecondIfLessOrEqual::DeallocateAll();
        }
    };

    template <typename T, MemoryAllocator Alloc>
    class StdAllocator: private Alloc
    {
    public:
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using void_pointer = void*;
        using const_void_pointer = const void*;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        template <typename U>
        struct rebind {
            using other = StdAllocator<U, Alloc>;
        };

        StdAllocator() = default;

        template <typename U>
        StdAllocator(const StdAllocator<U, Alloc>&) {}

        pointer allocate(size_type n)
        {
            if (n > std::numeric_limits<size_type>::max() / sizeof(T))
                throw std::bad_alloc();

            MemoryBlock block = Alloc::Allocate(n * sizeof(T));
            return reinterpret_cast<pointer>(block.Ptr);
        }

        void deallocate(pointer p, size_type n)
        {
            Alloc::Deallocate(MemoryBlock{p, n * sizeof(T)});
        }

        template <class U, class... Args>
        void construct(U* p, Args&&... args)
        {
            ::new((void*)p) U(std::forward<Args>(args)...);
        }

        template <class U>
        void destroy(U* p)
        {
            p->~U();
        }
    };

    template <typename T, MemoryAllocator U, MemoryAllocator V>
    bool operator==(const StdAllocator<T, U>&, const StdAllocator<T, V>&) {
        return std::is_same_v<U, V>;
    }

    template <typename T, MemoryAllocator U, MemoryAllocator V>
    bool operator!=(const StdAllocator<T, U>& a, const StdAllocator<T, V>& b) {
        return !(a == b);
    }
}