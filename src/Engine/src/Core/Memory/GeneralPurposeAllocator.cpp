//
// Created by Александр Лебедев on 16.05.2023.
//

#include "GeneralPurposeAllocator.h"
#include <cstdlib>
#ifdef DEBUG
#include <iostream>
#endif
#include "MallocAndFree.h"
#include "memory"
#include "Core/Logging/Log.h"
#include "Debug/Instrumentor.h"

#ifdef WINDOWS
#define STANDART_MEMORY_SIZE  104857600//1073741824
#else
#define STANDART_MEMORY_SIZE 104857600
#endif
//#define BLOCK_HEADER_SIZE 9 //4 байта unsigned int - размер блока, 1 байт - bool false/true - занят/свободен, 4 байта unsigned int - размер блока до этого
//#define BLOCK_SIZE_SIZE 4
//#define BEE_MEMORY_ALIGNMENT 32

inline void* align(int alignment, void* pVoid, size_t& size)
{
    void* ptr = (void*)(((uintptr_t)(pVoid) + alignment - 1) & ~(alignment - 1));
    size += ((uintptr_t)ptr - (uintptr_t)pVoid);
    return ptr;
}

AllocatorStatistics GeneralPurposeAllocator::s_Statistics;

void *AllocatorBlockHeader::StartWithoutAlignment(std::atomic<AllocatorBlockHeader>* ptr)
{
    return (void*)((unsigned char*)(ptr) + sizeof(std::atomic<AllocatorBlockHeader>));
}

void* AllocatorBlockHeader::Start(std::atomic<AllocatorBlockHeader>* ptr, unsigned int alignment)
{
    //(void*)(((uintptr_t)(this) + sizeof(BlockHeader) + MEMORY_ALIGNMENT - 1) & ~(MEMORY_ALIGNMENT));
    size_t S = size;
    void* aligned_ptr = ::align(alignment, StartWithoutAlignment(ptr), S);
#ifdef DEBUG
    if((uintptr_t)aligned_ptr%alignment != 0)
    {
        std::cout << "Wrong alignment!" << std::endl;
    }
#endif
    return aligned_ptr; //(void*)((unsigned char*)(&size) + BLOCK_HEADER_SIZE);
}

void AllocatorBlockHeader::align(int alignment, unsigned int sizeOfObject, void *&pVoid, size_t& sizeOfBlock)
{
    void* ptr = (void*)(((uintptr_t)(pVoid) + alignment - 1) & ~(alignment - 1));
    sizeOfBlock -= (uintptr_t)ptr - (uintptr_t)pVoid;
    pVoid = ptr;
}

std::atomic<AllocatorBlockHeader>* AllocatorBlockHeader::Previous(std::atomic<AllocatorBlockHeader>* ptr)
{
    return (std::atomic<AllocatorBlockHeader>*)((unsigned char*)ptr - sizeof(std::atomic<AllocatorBlockHeader>) - previousSize);
}

#define BLOCK_HEADER_SIZE sizeof(std::atomic<AllocatorBlockHeader>)

enum CombineBlocks
{
    No = 0,
    Previous = 1,
    Next = 2,
    Both = 3,
};

GeneralPurposeAllocator GeneralPurposeAllocator::s_Instance = GeneralPurposeAllocator();

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
    std::atomic<AllocatorBlockHeader>* blockHeaderPtr = (std::atomic<AllocatorBlockHeader>*)m_Memory->ptr;
    AllocatorBlockHeader blockHeader = blockHeaderPtr->load();
    Node* memoryNode = m_Memory;
    bool blockWasFound = false;
    size_t originalSize = size;
    size = size + alignment * 2 - 1 * 2;


    while (!blockWasFound)
    {
        while (blockHeader.size < size + BLOCK_HEADER_SIZE || !blockHeader.isFree)
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
            unsigned char* newMemory = (unsigned char*)bee_malloc(STANDART_MEMORY_SIZE);
            if (!newMemory)
            {
                std::cout<<"GeneralPurposeAllocator: Not enough memory"<<std::endl;
                return nullptr;
            }
            memoryNode->next = (Node*)bee_malloc(sizeof(Node));
            *memoryNode->next = Node(newMemory);
            InitializeMemory(newMemory, STANDART_MEMORY_SIZE);
        }
        memoryNode = memoryNode->next;
        blockHeaderPtr = (std::atomic<AllocatorBlockHeader>*)memoryNode->ptr;
        blockHeader = blockHeaderPtr->load();
    }
    AllocatorBlockHeader originalBlockHeader = blockHeader;
    if(blockHeader.size/(size + BLOCK_HEADER_SIZE) > 1 && blockHeader.size - (size + BLOCK_HEADER_SIZE * 2 + alignment-1) >= 1)
    {
        std::atomic<AllocatorBlockHeader>* nextNextBlockHeaderPtr = blockHeader.Next(blockHeaderPtr);
        AllocatorBlockHeader nextNextBlockHeader = nextNextBlockHeaderPtr->load();
        AllocatorBlockHeader originalNextNextBlockHeader = nextNextBlockHeader;
        blockHeader.size = size;
        std::atomic<AllocatorBlockHeader>* nextBlockHeaderPtr = (std::atomic<AllocatorBlockHeader> *)(align(
                alignment, blockHeader.Next(blockHeaderPtr), size));
        AllocatorBlockHeader originalNextBlockHeader = nextBlockHeaderPtr->load();
        blockHeader.size = size;
        blockHeader.isFree = false;
        AllocatorBlockHeader nextBlockHeader(nextNextBlockHeader.previousSize - blockHeader.size - BLOCK_HEADER_SIZE, true, size);

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

void GeneralPurposeAllocator::MergeFreeBlocks(AllocatorBlockHeader *blockHeader, int blocks) const
{

}

void GeneralPurposeAllocator::Initialize()
{
    if (m_Memory)
        return;
    m_Memory = (Node*)bee_malloc(sizeof(Node));
    *m_Memory = Node((unsigned char*)bee_malloc(STANDART_MEMORY_SIZE));
    if (!m_Memory->ptr)
    {
        std::cout<<"Unable to allocate memory"<<std::endl;
    }

    InitializeMemory(m_Memory->ptr, STANDART_MEMORY_SIZE);

#if 0
    TestAllocate();
    TestALlocateAndFree();
#endif
}

void GeneralPurposeAllocator::InitializeMemory(unsigned char *memory, size_t size)
{
    std::atomic<AllocatorBlockHeader>* blockHeaderPtr = (std::atomic<AllocatorBlockHeader>*)memory;
    AllocatorBlockHeader blockHeader = blockHeaderPtr->load();
    blockHeader.size = size - BLOCK_HEADER_SIZE * 2 - alignof(std::max_align_t);
    blockHeader.isFree = true;
    blockHeader.previousSize = 0;

    size = blockHeader.size;
    std::atomic<AllocatorBlockHeader>* nextBlockHeaderPtr = (std::atomic<AllocatorBlockHeader>*)align(alignof(std::max_align_t),blockHeader.Next(blockHeaderPtr), size);
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
    while (!(currentMemory == nullptr || currentMemory->ptr <= pVoid && pVoid < currentMemory->ptr + STANDART_MEMORY_SIZE))
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


#if 0
void GeneralPurposeAllocator::TestAllocate()
{
    // Тест 1: Выделение памяти и проверка корректности выделенной памяти, memory alignment и BlockHeader
    unsigned int size = 100;
    void* ptr = GeneralPurposeAllocator::Allocate(size, 8);
    if (ptr != nullptr)
    {
        // Проверка корректности выделенной памяти
        AllocatorBlockHeader* blockHeader = static_cast<AllocatorBlockHeader*>(GeneralPurposeAllocator::s_Instance.FindBlockHeader(ptr));
        if (blockHeader != nullptr && blockHeader->size >= size)
        {
            std::cout << "Test 1: Memory allocation successful. Allocated size: " << blockHeader->size << std::endl;
            // Проверка memory alignment
            if ((reinterpret_cast<uintptr_t>(ptr) & (8 - 1)) == 0)
            {
                std::cout << "Test 1: Memory alignment is correct." << std::endl;
            }
            else
            {
                std::cout << "Test 1: Memory alignment is incorrect." << std::endl;
            }
            // Проверка данных в BlockHeader
            if (blockHeader->isFree == false && blockHeader->previousSize == 0)
            {
                std::cout << "Test 1: BlockHeader data is correct." << std::endl;
            }
            else
            {
                std::cout << "Test 1: BlockHeader data is incorrect." << std::endl;
            }
            GeneralPurposeAllocator::Free(ptr);
            if (blockHeader->isFree && blockHeader->previousSize == 0 && blockHeader->size == STANDART_MEMORY_SIZE - BLOCK_HEADER_SIZE * 2)
            {
                std::cout << "Test 1: BlockHeader data is correct after Freeing." << std::endl;
            }
            else
            {
                std::cout << "Test 1: BlockHeader data is incorrect after Freeing." << std::endl;
            }
        }
        else
        {
            std::cout << "Test 1: Memory allocation failed or incorrect block header size." << std::endl;
        }
    }
    else
    {
        std::cout << "Test 1: Memory allocation failed." << std::endl;
    }
}

void GeneralPurposeAllocator::TestALlocateAndFree()
{
    // Тест 2: Выделение и освобождение нескольких блоков памяти и проверка целостности BlockHeader
    unsigned int size1 = 100;
    unsigned int size2 = 200;

    // Выделение первого блока памяти
    void* ptr1 = GeneralPurposeAllocator::Allocate(size1, 8);
    if (ptr1 != nullptr)
    {
        AllocatorBlockHeader* blockHeader1 = static_cast<AllocatorBlockHeader*>(GeneralPurposeAllocator::s_Instance.FindBlockHeader(ptr1));
        if (blockHeader1 != nullptr)
        {
            std::cout << "Test 2: Memory 1 allocation successful. Allocated size: " << blockHeader1->size << std::endl;
        }
        else
        {
            std::cout << "Test 2: Memory 1 allocation failed. Invalid block header." << std::endl;
            return;
        }
    }
    else
    {
        std::cout << "Test 2: Memory 1 allocation failed." << std::endl;
        return;
    }

    // Выделение второго блока памяти
    void* ptr2 = GeneralPurposeAllocator::Allocate(size2, 8);
    if (ptr2 != nullptr)
    {
        AllocatorBlockHeader* blockHeader2 = static_cast<AllocatorBlockHeader*>(GeneralPurposeAllocator::s_Instance.FindBlockHeader(ptr2));
        if (blockHeader2 != nullptr)
        {
            std::cout << "Test 2: Memory 2 allocation successful. Allocated size: " << blockHeader2->size << std::endl;
        }
        else
        {
            std::cout << "Test 2: Memory 2 allocation failed. Invalid block header." << std::endl;
            return;
        }
        AllocatorBlockHeader* blockHeader1 = static_cast<AllocatorBlockHeader*>(GeneralPurposeAllocator::s_Instance.FindBlockHeader(ptr1));
        if (blockHeader1 != nullptr && !blockHeader1->isFree && blockHeader1->size >= size1 && blockHeader1->previousSize == 0)
        {
            std::cout << "Test 2: Memory 1 is correct." << std::endl;
        }
        else
        {
            std::cout << "Test 2: Memory 1 is incorrect." << std::endl;
            return;
        }

        if(blockHeader2->previousSize == blockHeader1->size)
        {
            std::cout << "Test 2: Memory 2 previous size is correct." << std::endl;
        }
        else
        {
            std::cout << "Test 2: Memory 2 previous size is incorrect." << std::endl;
            return;
        }

        if(!blockHeader2->Next()->Next()->isFree && blockHeader2->Next()->Next()->size == 0)
        {
            std::cout << "Test 2: Memory 2 block is correct." << std::endl;
        }
        else
        {
            std::cout << "Test 2: Memory 2 block is incorrect." << std::endl;
            return;
        }
    }
    else
    {
        std::cout << "Test 2: Memory 2 allocation failed." << std::endl;
        return;
    }

    // Освобождение первого блока памяти
    GeneralPurposeAllocator::Free(ptr1);
    std::cout << "Test 2: Memory 1 freed." << std::endl;

    // Освобождение второго блока памяти
    GeneralPurposeAllocator::Free(ptr2);
    std::cout << "Test 2: Memory 2 freed." << std::endl;

    // Проверка целостности BlockHeader после освобождения памяти
    AllocatorBlockHeader* blockHeader1 = static_cast<AllocatorBlockHeader*>(GeneralPurposeAllocator::s_Instance.FindBlockHeader(ptr1));
    if (blockHeader1 != nullptr && blockHeader1->isFree && blockHeader1->size == STANDART_MEMORY_SIZE - BLOCK_HEADER_SIZE*2 && blockHeader1->previousSize == 0)
    {
        std::cout << "Test 2: BlockHeader 1 is intact." << std::endl;
    }
    else
    {
        std::cout << "Test 2: BlockHeader 1 is corrupted or memory not freed properly." << std::endl;
    }

    if(!blockHeader1->Next()->isFree && blockHeader1->Next()->size == 0 && blockHeader1->Next()->previousSize == blockHeader1->size)
    {
        std::cout << "Test 2: Last BlockHeader is intact." << std::endl;
    }
    else
    {
        std::cout << "Test 2: Last BlockHeader is corrupted." << std::endl;
    }
}
#endif
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

std::atomic<AllocatorBlockHeader>* AllocatorBlockHeader::Next(std::atomic<AllocatorBlockHeader>* ptr)
{
    return (std::atomic<AllocatorBlockHeader>*)(((uintptr_t)ptr) + sizeof(std::atomic<AllocatorBlockHeader>) + size);
}
