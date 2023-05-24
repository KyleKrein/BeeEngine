//
// Created by Александр Лебедев on 16.05.2023.
//



#include "GeneralPurposeAllocator.h"
#include <cstddef>
#include "MallocAndFree.h"


#ifdef WINDOWS
#define BEE_STANDART_MEMORY_SIZE  104857600
#else
#define STANDART_MEMORY_SIZE 104857600
#endif

namespace BeeEngine
{
    namespace Internal
    {
        AllocatorStatistics GeneralPurposeAllocator::s_Statistics;

#if 1

#define BEE_BLOCK_HEADER_SIZE sizeof(std::atomic<AllocatorBlockHeader>)

        enum CombineBlocks
        {
            No = 0,
            Previous = 1,
            Next = 2,
            Both = 3,
        };

        GeneralPurposeAllocator GeneralPurposeAllocator::s_Instance = GeneralPurposeAllocator();

        void GeneralPurposeAllocator::Initialize()
        {
            if (m_Memory)
                return;
            m_Memory = (Node*)bee_malloc(sizeof(Node));
            *m_Memory = Node((unsigned char*)bee_malloc(BEE_STANDART_MEMORY_SIZE));
            if (!m_Memory->ptr)
            {
                std::cout<<"Unable to allocate memory"<<std::endl;
            }

            InitializeMemory(m_Memory->ptr, BEE_STANDART_MEMORY_SIZE);

#if 0
            TestAllocate();
    TestALlocateAndFree();
#endif
        }

        void GeneralPurposeAllocator::InitializeMemory(unsigned char *memory, size_t size)
        {
            std::atomic<AllocatorBlockHeader>* blockHeaderPtr = (std::atomic<AllocatorBlockHeader>*)memory;
            AllocatorBlockHeader blockHeader = blockHeaderPtr->load();
            blockHeader.size = size - BEE_BLOCK_HEADER_SIZE * 2 - alignof(std::max_align_t);
            blockHeader.isFree = true;
            blockHeader.previousSize = 0;

            size = blockHeader.size;
            auto* nextBlockHeaderPtr = (std::atomic<AllocatorBlockHeader>*)AlignMemoryAddress(alignof(std::max_align_t),blockHeader.Next(blockHeaderPtr), size);
            blockHeader.size = size;
            AllocatorBlockHeader nextBlockHeader = nextBlockHeaderPtr->load();
            nextBlockHeader.size = 0;
            nextBlockHeader.isFree = false;
            nextBlockHeader.previousSize = blockHeader.size;

            blockHeaderPtr->store(blockHeader);
            nextBlockHeaderPtr->store(nextBlockHeader);

            s_Statistics.totalMemoryPages++;
            s_Statistics.freeBlocks++;
        }

        void* GeneralPurposeAllocator::FindBlockHeader(void *pVoid)
        {
            Node* currentMemory = m_Memory;
            while (!(currentMemory == nullptr || currentMemory->ptr <= pVoid && pVoid < currentMemory->ptr + BEE_STANDART_MEMORY_SIZE))
            {
                currentMemory = currentMemory->next;
            }

            if (!currentMemory)
            {
                std::cout<<"Unable to free memory: memory was allocated with different allocator"<<std::endl;
                throw std::exception();
            }

            std::atomic<AllocatorBlockHeader>* currentBlockPtr = reinterpret_cast<std::atomic<AllocatorBlockHeader>*>(currentMemory->ptr);
            AllocatorBlockHeader currentBlock = currentBlockPtr->load();
            while (reinterpret_cast<unsigned char*>(currentBlock.StartWithoutAlignment(currentBlockPtr)) + currentBlock.size < reinterpret_cast<unsigned char*>(pVoid))
            {
                currentBlockPtr = currentBlock.Next(currentBlockPtr);
                currentBlock = currentBlockPtr->load();
            }

            return currentBlockPtr;
        }

        void *GeneralPurposeAllocator::AllocateMemory(size_t size, size_t alignment)
        {
            if(m_Memory == nullptr)
            {
                throw std::runtime_error("GeneralPurposeAllocator: Memory is not initialized");
            }
            if(size == 0)
            {
                return nullptr;
            }
            auto* blockHeaderPtr = (std::atomic<AllocatorBlockHeader>*)m_Memory->ptr;
            AllocatorBlockHeader blockHeader = blockHeaderPtr->load();
            Node* memoryNode = m_Memory;
            bool blockWasFound = false;
            size_t originalSize = size;
            size = size + alignment * 2 - 1 * 2;


            while (!blockWasFound)
            {
                while (blockHeader.size < size + BEE_BLOCK_HEADER_SIZE || !blockHeader.isFree)
                {
                    blockHeaderPtr = blockHeader.Next(blockHeaderPtr);
                    blockHeader = blockHeaderPtr->load();
                    if (blockHeader.size == 0)
                    {
                        break;
                    }
                }
                if (blockHeader.size)
                {
                    blockWasFound = true;
                    break;
                }
                if (!memoryNode->next)
                {
                    unsigned char* newMemory = (unsigned char*)bee_malloc(BEE_STANDART_MEMORY_SIZE);
                    if (!newMemory)
                    {
                        std::cout<<"GeneralPurposeAllocator: Not enough memory"<<std::endl;
                        return nullptr;
                    }
                    memoryNode->next = (Node*)bee_malloc(sizeof(Node));
                    *memoryNode->next = Node(newMemory);
                    InitializeMemory(newMemory, BEE_STANDART_MEMORY_SIZE);
                }
                memoryNode = memoryNode->next;
                blockHeaderPtr = (std::atomic<AllocatorBlockHeader>*)memoryNode->ptr;
                blockHeader = blockHeaderPtr->load();
            }
            AllocatorBlockHeader originalBlockHeader = blockHeader;
            if(blockHeader.size/(size + BEE_BLOCK_HEADER_SIZE) > 1 && blockHeader.size - (size + BEE_BLOCK_HEADER_SIZE * 2 + alignment - 1) >= 1)
            {
                std::atomic<AllocatorBlockHeader>* nextNextBlockHeaderPtr = blockHeader.Next(blockHeaderPtr);
                AllocatorBlockHeader nextNextBlockHeader = nextNextBlockHeaderPtr->load();
                AllocatorBlockHeader originalNextNextBlockHeader = nextNextBlockHeader;
                blockHeader.size = size;
                std::atomic<AllocatorBlockHeader>* nextBlockHeaderPtr = (std::atomic<AllocatorBlockHeader> *)(AlignMemoryAddress(
                        alignment, blockHeader.Next(blockHeaderPtr), size));
                AllocatorBlockHeader originalNextBlockHeader = nextBlockHeaderPtr->load();
                blockHeader.size = size;
                blockHeader.isFree = false;
                AllocatorBlockHeader nextBlockHeader(nextNextBlockHeader.previousSize - blockHeader.size - BEE_BLOCK_HEADER_SIZE, true, size);

#ifdef DEBUG
                if(size != blockHeader.size)
                {
                    std::cout << "GeneralPurposeAllocator: Incorrect size in block. Size was: " <<size<<" and now is "<< blockHeader.size << std::endl;
                }
#endif
                nextBlockHeader.previousSize = blockHeader.size;
                nextNextBlockHeader.previousSize = nextBlockHeader.size;
                //int difference = memcmp(blockHeaderPtr, &originalBlockHeader, sizeof(AllocatorBlockHeader));
                if(blockHeaderPtr->compare_exchange_strong(originalBlockHeader, blockHeader))
                {
                    if(nextBlockHeaderPtr->compare_exchange_strong(originalNextBlockHeader, nextBlockHeader))
                    {
                        if(!nextNextBlockHeaderPtr->compare_exchange_strong(originalNextNextBlockHeader, nextNextBlockHeader))
                        {
                            if(nextBlockHeaderPtr->compare_exchange_strong(nextBlockHeader, originalNextBlockHeader))
                                std::cout<<"GeneralPurposeAllocator: Critical failure! Memory was corrupted"<<std::endl;
                            if(blockHeaderPtr->compare_exchange_strong(blockHeader, originalBlockHeader))
                                std::cout<<"GeneralPurposeAllocator: Critical failure! Memory was corrupted"<<std::endl;
                            goto FAILURE;
                        }
                    }
                    else
                    {
                        if(blockHeaderPtr->compare_exchange_strong(blockHeader, originalBlockHeader))
                            std::cout<<"GeneralPurposeAllocator: Critical failure! Memory was corrupted"<<std::endl;
                        goto FAILURE;
                    }
                }
                else
                {
                    FAILURE:
                    return AllocateMemory(originalSize, alignment);
                }


#ifdef DEBUG
                if (nextBlockHeader.size != nextNextBlockHeader.previousSize)
                {
                    std::cout << "GeneralPurposeAllocator: Incorrect size in block" << std::endl;
                }
                if(nextBlockHeaderPtr->load().Next(nextBlockHeaderPtr) != nextNextBlockHeaderPtr)
                {
                    std::cout << "GeneralPurposeAllocator: BlockHeader->Next and Next BlockHeader don't match pointers" << std::endl;
                }
                if(nextNextBlockHeaderPtr->load().Previous(nextNextBlockHeaderPtr) != nextBlockHeaderPtr)
                {
                    std::cout << "GeneralPurposeAllocator: BlockHeader->Next and Next BlockHeader don't match pointers 2" << std::endl;
                }
                if(blockHeaderPtr->load().Next(blockHeaderPtr) != nextBlockHeaderPtr)
                {
                    std::cout << "GeneralPurposeAllocator: BlockHeader->Next and Next BlockHeader don't match pointers 3" << std::endl;
                }
                if(nextBlockHeaderPtr->load().Previous(nextBlockHeaderPtr) != blockHeaderPtr)
                {
                    std::cout << "GeneralPurposeAllocator: BlockHeader->Next and Next BlockHeader don't match pointers 4" << std::endl;
                }
#endif
            }
            else
            {
                blockHeader.isFree = false;
                if (!blockHeaderPtr->compare_exchange_strong(originalBlockHeader, blockHeader))
                {
                    return AllocateMemory(originalSize, alignment);
                }
            }

            s_Statistics.allocatedBlocks++;
            s_Statistics.totalAllocatedBlocks++;
            s_Statistics.allocatedMemory += blockHeader.size;

            return blockHeader.Start(blockHeaderPtr, alignment);
        }

        void GeneralPurposeAllocator::FreeMemory(void *ptr)
        {
            std::atomic<AllocatorBlockHeader>* blockHeaderPtr = (std::atomic<AllocatorBlockHeader>*)FindBlockHeader(ptr);//(BlockHeader*)ptr - 1;//(BlockHeader*)((unsigned char*)*(uintptr_t*)((unsigned char*)(ptr) - sizeof(void*))- BLOCK_HEADER_SIZE);

            AllocatorBlockHeader blockHeader = blockHeaderPtr->load();
            AllocatorBlockHeader originalBlockHeader = blockHeader;
            if (blockHeader.isFree)
            {
                throw std::runtime_error("GeneralPurposeAllocator: Double free");
            }

            blockHeader.isFree = true;

            int blocks = CombineBlocks::No;


            std::atomic<AllocatorBlockHeader>* previousBlockHeaderPtr = blockHeader.Previous(blockHeaderPtr);
            AllocatorBlockHeader previousBlockHeader = previousBlockHeaderPtr->load();

            std::atomic<AllocatorBlockHeader>* nextBlockHeaderPtr = blockHeader.Next(blockHeaderPtr);
            AllocatorBlockHeader nextBlockHeader = nextBlockHeaderPtr->load();

            if(blockHeader.previousSize && previousBlockHeader.isFree)
            {
                blocks |= CombineBlocks::Previous;
            }

            if (nextBlockHeader.size && nextBlockHeader.isFree)
            {
                blocks |= CombineBlocks::Next;
            }

            s_Statistics.allocatedBlocks--;
            s_Statistics.totalFreedBlocks++;
            s_Statistics.allocatedMemory -= blockHeader.size;
            s_Statistics.totalFreedMemory += blockHeader.size;

            if (blocks == 0)
            {
                if(!blockHeaderPtr->compare_exchange_strong(originalBlockHeader, blockHeader))
                {
                    FreeMemory(ptr);
                    return;
                }
                s_Statistics.freeBlocks++;
                return;
            }
            unsigned int resultSize;
            if (blocks == 1)
            {
                resultSize = (uintptr_t)nextBlockHeaderPtr - (uintptr_t)previousBlockHeader.StartWithoutAlignment(previousBlockHeaderPtr);
                previousBlockHeader.size = resultSize;
                nextBlockHeader.previousSize = resultSize;

                auto originalPreviousBlockHeader = previousBlockHeader;
                auto originalNextBlockHeader = nextBlockHeader;
                if (!previousBlockHeaderPtr->compare_exchange_strong(originalPreviousBlockHeader, previousBlockHeader))
                {
                    FreeMemory(ptr);
                    return;
                }
                if (!nextBlockHeaderPtr->compare_exchange_strong(originalNextBlockHeader, nextBlockHeader))
                {
                    if (!previousBlockHeaderPtr->compare_exchange_strong(previousBlockHeader, originalPreviousBlockHeader))
                    {
                        std::cout << "GeneralPurposeAllocator: Critical failure! Memory was corrupted" << std::endl;
                    }
                    FreeMemory(ptr);
                    return;
                }


#ifdef DEBUG
                if (nextBlockHeaderPtr != previousBlockHeader.Next(previousBlockHeaderPtr))
                {
                    std::cout << "GeneralPurposeAllocator: BlockHeader and BlockHeader->Next() don't match pointers after combining" << std::endl;
                }
#endif

                s_Statistics.blocksCombined++;
                return;
            }


            std::atomic<AllocatorBlockHeader>* nextNextBlockHeaderPtr = nextBlockHeader.Next(nextBlockHeaderPtr);
            AllocatorBlockHeader nextNextBlockHeader = nextNextBlockHeaderPtr->load();
            AllocatorBlockHeader originalNextNextBlockHeader = nextNextBlockHeader;

            switch ((CombineBlocks)blocks)
            {
                case Next:
                    resultSize = (uintptr_t)nextNextBlockHeaderPtr - (uintptr_t)blockHeader.StartWithoutAlignment(blockHeaderPtr);
                    blockHeader.size = resultSize;
                    nextNextBlockHeader.previousSize = resultSize;

                    if (!blockHeaderPtr->compare_exchange_strong(originalBlockHeader, blockHeader))
                    {
                        FreeMemory(ptr);
                        return;
                    }
                    if (!nextNextBlockHeaderPtr->compare_exchange_strong(originalNextNextBlockHeader, nextNextBlockHeader))
                    {
                        if (!blockHeaderPtr->compare_exchange_strong(blockHeader, originalBlockHeader))
                        {
                            std::cout << "GeneralPurposeAllocator: Critical failure! Memory was corrupted" << std::endl;
                        }
                        FreeMemory(ptr);
                        return;
                    }


#ifdef DEBUG
                    if (nextNextBlockHeaderPtr != blockHeader.Next(blockHeaderPtr))
                    {
                        std::cout << "GeneralPurposeAllocator: BlockHeader and BlockHeader->Next() don't match pointers after combining" << std::endl;
                    }
#endif

                    s_Statistics.blocksCombined++;
                    break;

                case Both:

                    auto originalPreviousBlockHeader = previousBlockHeader;

                    resultSize = (uintptr_t)nextNextBlockHeaderPtr - (uintptr_t)previousBlockHeader.StartWithoutAlignment(previousBlockHeaderPtr);
                    previousBlockHeader.size = resultSize;
                    nextNextBlockHeader.previousSize = resultSize;


                    if(!previousBlockHeaderPtr->compare_exchange_strong(originalPreviousBlockHeader, previousBlockHeader))
                    {
                        FreeMemory(ptr);
                        return;
                    }

                    if(!nextNextBlockHeaderPtr->compare_exchange_strong(originalNextNextBlockHeader, nextNextBlockHeader))
                    {
                        if (!previousBlockHeaderPtr->compare_exchange_strong(previousBlockHeader, originalPreviousBlockHeader))
                        {
                            std::cout << "GeneralPurposeAllocator: Critical failure! Memory was corrupted" << std::endl;
                        }
                        FreeMemory(ptr);
                        return;
                    }

#ifdef DEBUG
                    if (nextNextBlockHeaderPtr != previousBlockHeader.Next(previousBlockHeaderPtr))
                    {
                        std::cout << "GeneralPurposeAllocator: BlockHeader and BlockHeader->Next() don't match pointers after combining" << std::endl;
                    }
#endif

                    s_Statistics.blocksCombined+=2;
                    s_Statistics.freeBlocks--;
                    break;
            }

        }

        GeneralPurposeAllocator::GeneralPurposeAllocator()
        {
#ifdef DEBUG
            std::cout << "Allocator is Initializing." << std::endl;
#endif
            Initialize();
        }

        GeneralPurposeAllocator::~GeneralPurposeAllocator()
        {
#ifdef DEBUG
            std::cout << "Allocator is Shutting down." << std::endl;
            CheckForUnfreedMemory();
            PrintStatistics();
#endif
            Shutdown();
        }

        void *GeneralPurposeAllocator::Allocate(size_t size, size_t alignment)
        {
            return s_Instance.AllocateMemory(size, alignment);
        }

        void GeneralPurposeAllocator::Free(void *ptr)
        {
            s_Instance.FreeMemory(ptr);
        }

        void GeneralPurposeAllocator::Shutdown()
        {
            Node* ptr = m_Memory;
            while (ptr)
            {
                Node* next = ptr->next;
                bee_free(ptr->ptr);
                bee_free(ptr);
                ptr = next;
            }
        }

        void GeneralPurposeAllocator::CheckForUnfreedMemory()
        {
            Node* ptr = m_Memory;
            while (ptr)
            {
                std::atomic<AllocatorBlockHeader>* blockHeaderPtr = reinterpret_cast<std::atomic<AllocatorBlockHeader>*>(ptr->ptr);
                AllocatorBlockHeader blockHeader = blockHeaderPtr->load();
                while(blockHeader.size)
                {
                    if(!blockHeader.isFree)
                    {
                        std::cout << "Memory leak detected. Block size: " << blockHeader.size << std::endl;
                    }
                    blockHeaderPtr = blockHeader.Next(blockHeaderPtr);
                    blockHeader = blockHeaderPtr->load();
                }
                ptr = ptr->next;
            }
        }

        void GeneralPurposeAllocator::PrintStatistics()
        {
            std::cout << "Memory usage statistics:" << std::endl;
            std::cout<< "Total memory allocated: " << s_Statistics.allocatedMemory << std::endl;
        }

#endif
    }
}

