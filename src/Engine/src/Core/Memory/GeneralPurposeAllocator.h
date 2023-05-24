//
// Created by Александр Лебедев on 16.05.2023.
//

#pragma once
#include <atomic>
#include <iostream>

namespace BeeEngine
{
    namespace Internal
    {
        struct AllocatorStatistics
        {
            std::atomic<size_t> allocatedMemory;
            std::atomic<size_t> totalFreedMemory;
            std::atomic<size_t> totalAllocatedMemory;
            std::atomic<size_t> totalAllocatedBlocks;
            std::atomic<size_t> totalFreedBlocks;
            std::atomic<size_t> allocatedBlocks;
            std::atomic<size_t> freeBlocks;
            std::atomic<size_t> totalMemoryPages;
            std::atomic<size_t> blocksCombined;
        };

        inline void* AlignMemoryAddress(int alignment, void* pVoid, size_t& size)
        {
            void* ptr = (void*)(((uintptr_t)(pVoid) + alignment - 1) & ~(alignment - 1));
            size += ((uintptr_t)ptr - (uintptr_t)pVoid);
            return ptr;
        }

        struct AllocatorBlockHeader
        {
            uint32_t size;
            uint64_t isFree;
            uint32_t previousSize;
            AllocatorBlockHeader(unsigned int size, bool isFree, unsigned int previousSize)
                    :size(size), isFree(isFree), previousSize(previousSize) {}
            inline std::atomic<AllocatorBlockHeader>* Next(std::atomic<AllocatorBlockHeader>* ptr) const
            {
                return (std::atomic<AllocatorBlockHeader>*)(((uintptr_t)ptr) + sizeof(std::atomic<AllocatorBlockHeader>) + size);
            }
            inline std::atomic<AllocatorBlockHeader>* Previous(std::atomic<AllocatorBlockHeader>* ptr) const
            {
                return (std::atomic<AllocatorBlockHeader>*)((unsigned char*)ptr - sizeof(std::atomic<AllocatorBlockHeader>) - previousSize);
            }
            inline void* Start(std::atomic<AllocatorBlockHeader>* ptr, unsigned int alignment)
            {
                size_t S = size;
                void* aligned_ptr = AlignMemoryAddress(alignment, StartWithoutAlignment(ptr), S);
#ifdef DEBUG
                if((uintptr_t)aligned_ptr%alignment != 0)
    {
        std::cout << "Wrong alignment!" << std::endl;
    }
#endif
                return aligned_ptr;
            }

            inline void* StartWithoutAlignment(std::atomic<AllocatorBlockHeader>* ptr)
            {
                return (void*)((unsigned char*)(ptr) + sizeof(std::atomic<AllocatorBlockHeader>));
            }
        };



        class GeneralPurposeAllocator
        {
            friend class GeneralPurposeAllocatorTest;
        public:
            GeneralPurposeAllocator();
            ~GeneralPurposeAllocator();
            void Initialize();
            static void* Allocate(size_t size, size_t alignment);
            void* AllocateMemory(size_t size, size_t alignment);
            static void Free(void* ptr);
            void FreeMemory(void* ptr);
            void Shutdown();

            inline static const AllocatorStatistics& GetStatistics()
            {
                return s_Statistics;
            }
        private:
            struct Node
            {
                unsigned char* ptr;
                Node* next;
                Node(unsigned char* pointer)
                        :ptr(pointer), next(nullptr) {}
            };
            Node* m_Memory = nullptr;
            static AllocatorStatistics s_Statistics;
            //static void(*LogCallbackFunc)(const char* message);

            void InitializeMemory(unsigned char *memory, size_t size);

            void* FindBlockHeader(void *pVoid);

            static GeneralPurposeAllocator s_Instance;

            void CheckForUnfreedMemory();

            void PrintStatistics();
        };
    }
}