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

#ifdef WINDOWS
#define STANDART_MEMORY_SIZE  1073741824
#else
#define STANDART_MEMORY_SIZE 104857600
#endif
//#define BLOCK_HEADER_SIZE 9 //4 байта unsigned int - размер блока, 1 байт - bool false/true - занят/свободен, 4 байта unsigned int - размер блока до этого
#define BLOCK_SIZE_SIZE 4
#define MEMORY_ALIGNMENT 8

AllocatorStatistics GeneralPurposeAllocator::s_Statistics;

void *AllocatorBlockHeader::StartWithoutAlignment()
{
    return (void*)((unsigned char*)(&size) + sizeof(AllocatorBlockHeader));
}

void *AllocatorBlockHeader::Start()
{
    void* aligned_ptr = (unsigned char*)(&this->size) + sizeof(AllocatorBlockHeader);//(void*)(((uintptr_t)(this) + sizeof(BlockHeader) + MEMORY_ALIGNMENT - 1) & ~(MEMORY_ALIGNMENT));
    size_t S = size;
    std::align(MEMORY_ALIGNMENT, size-MEMORY_ALIGNMENT+1, aligned_ptr, S);
    BeeCoreAssert(alignof(aligned_ptr) == MEMORY_ALIGNMENT, "Wrong alignment");
    return aligned_ptr; //(void*)((unsigned char*)(&size) + BLOCK_HEADER_SIZE);
}

void AllocatorBlockHeader::align(int alignment, unsigned int sizeOfObject, void *&pVoid, unsigned int sizeOfBlock)
{
    pVoid = (void*)(((uintptr_t)(pVoid) + alignment - 1) & ~(alignment - 1));
}

AllocatorBlockHeader *AllocatorBlockHeader::Previous()
{
    return (AllocatorBlockHeader*)((unsigned char*)this - sizeof(AllocatorBlockHeader) - previousSize);
}

#define BLOCK_HEADER_SIZE sizeof(AllocatorBlockHeader)

enum CombineBlocks
{
    No = 0,
    Previous = 1,
    Next = 2,
    Both = 3,
};

thread_local GeneralPurposeAllocator GeneralPurposeAllocator::s_Instance = GeneralPurposeAllocator();

void *GeneralPurposeAllocator::AllocateMemory(size_t size, unsigned int alignment)
{
    if(size == 0)
    {
        return nullptr;
    }

    AllocatorBlockHeader* blockHeader = (AllocatorBlockHeader*)m_Memory->ptr;
    Node* memoryNode = m_Memory;
    bool blockWasFound = false;

    size = size + MEMORY_ALIGNMENT - 1;


    while (!blockWasFound)
    {
        while (blockHeader->size < size + BLOCK_HEADER_SIZE || !blockHeader->isFree)
        {
            blockHeader = blockHeader->Next();
            if (blockHeader->size == 0)
            {
                break;
            }
        }
        if (blockHeader->size)
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
        blockHeader = (AllocatorBlockHeader*)memoryNode->ptr;
    }

    if(blockHeader->size/(size + BLOCK_HEADER_SIZE) > 1 && blockHeader->size - (size + BLOCK_HEADER_SIZE * 2) >= 1)
    {
        AllocatorBlockHeader* nextNextBlockHeader = blockHeader->Next();
        blockHeader->size = size;
        blockHeader->Next()->previousSize = size;
        blockHeader->Next()->size = (uintptr_t)nextNextBlockHeader - (uintptr_t)blockHeader->Next() - BLOCK_HEADER_SIZE;
        blockHeader->Next()->isFree = true;
        nextNextBlockHeader->previousSize = blockHeader->Next()->size;

#ifdef DEBUG
        if (blockHeader->Next()->size != nextNextBlockHeader->previousSize)
        {
            std::cout << "GeneralPurposeAllocator: Incorrect size in block" << std::endl;
        }
        if(blockHeader->Next()->Next() != nextNextBlockHeader)
        {
            std::cout << "GeneralPurposeAllocator: BlockHeader->Next and Next BlockHeader don't match pointers" << std::endl;
        }
#endif
    }
    blockHeader->isFree = false;

    s_Statistics.allocatedBlocks++;
    s_Statistics.totalAllocatedBlocks++;
    s_Statistics.allocatedMemory += blockHeader->size;

    return blockHeader->Start();
}

void GeneralPurposeAllocator::FreeMemory(void *ptr)
{

    AllocatorBlockHeader* blockHeader = (AllocatorBlockHeader*)FindBlockHeader(ptr);//(BlockHeader*)ptr - 1;//(BlockHeader*)((unsigned char*)*(uintptr_t*)((unsigned char*)(ptr) - sizeof(void*))- BLOCK_HEADER_SIZE);

    if (blockHeader->isFree)
    {
        std::cout<<"GeneralPurposeAllocator: Double free" << std::endl;
        return;
    }

    blockHeader->isFree = true;

    int blocks = CombineBlocks::No;

    if(blockHeader->previousSize && blockHeader->Previous()->isFree)
    {
        blocks |= CombineBlocks::Previous;
    }

    if (blockHeader->Next()->size && blockHeader->Next()->isFree)
    {
        blocks |= CombineBlocks::Next;
    }

    s_Statistics.allocatedBlocks--;
    s_Statistics.totalFreedBlocks++;
    s_Statistics.allocatedMemory -= blockHeader->size;
    s_Statistics.freedMemory += blockHeader->size;

    unsigned int resultSize;
    AllocatorBlockHeader* nextNextBlockHeader;
    AllocatorBlockHeader* previousBlockHeader;


    switch ((CombineBlocks)blocks)
    {
        case CombineBlocks::No:
            s_Statistics.freeBlocks++;
            return;
        case CombineBlocks::Previous:
            previousBlockHeader = blockHeader->Previous();
            nextNextBlockHeader = blockHeader->Next();
            resultSize = (uintptr_t)nextNextBlockHeader - (uintptr_t)previousBlockHeader->StartWithoutAlignment();
            previousBlockHeader->size = resultSize;
            nextNextBlockHeader->previousSize = resultSize;

#ifdef DEBUG
            if (nextNextBlockHeader != previousBlockHeader->Next())
            {
                std::cout << "GeneralPurposeAllocator: BlockHeader and BlockHeader->Next() don't match pointers after combining" << std::endl;
            }
#endif

            s_Statistics.blocksCombined++;
            break;
        case CombineBlocks::Next:
            nextNextBlockHeader = blockHeader->Next()->Next();
            resultSize = (uintptr_t)nextNextBlockHeader - (uintptr_t)blockHeader->StartWithoutAlignment();
            blockHeader->size = resultSize;
            nextNextBlockHeader->previousSize = resultSize;

#ifdef DEBUG
            if (nextNextBlockHeader != blockHeader->Next())
            {
                std::cout << "GeneralPurposeAllocator: BlockHeader and BlockHeader->Next() don't match pointers after combining" << std::endl;
            }
#endif

            s_Statistics.blocksCombined++;
            break;
        case CombineBlocks::Both:
            nextNextBlockHeader = blockHeader->Next()->Next();
            resultSize = (uintptr_t)nextNextBlockHeader - (uintptr_t)blockHeader->Previous()->StartWithoutAlignment();
            previousBlockHeader = blockHeader->Previous();
            previousBlockHeader->size = resultSize;
            nextNextBlockHeader->previousSize = resultSize;

#ifdef DEBUG
            if (nextNextBlockHeader != previousBlockHeader->Next())
            {
                std::cout << "GeneralPurposeAllocator: BlockHeader and BlockHeader->Next() don't match pointers after combining" << std::endl;
            }
#endif

            s_Statistics.blocksCombined+=2;
            s_Statistics.freeBlocks--;
            break;
    }
}

void GeneralPurposeAllocator::Initialize()
{

    m_Memory = (Node*)bee_malloc(sizeof(Node));
    *m_Memory = Node((unsigned char*)bee_malloc(STANDART_MEMORY_SIZE));
    if (!m_Memory->ptr)
    {
        std::cout<<"Unable to allocate memory"<<std::endl;
    }

    InitializeMemory(m_Memory->ptr, STANDART_MEMORY_SIZE);

#ifdef DEBUG
    TestAllocate();
    TestALlocateAndFree();
#endif
}

void GeneralPurposeAllocator::InitializeMemory(unsigned char *memory, unsigned long long size)
{
    AllocatorBlockHeader* blockHeader = (AllocatorBlockHeader*)memory;
    blockHeader->size = size - BLOCK_HEADER_SIZE * 2;
    blockHeader->isFree = true;
    blockHeader->previousSize = 0;
    blockHeader->Next()->size = 0;
    blockHeader->Next()->isFree = false;
    blockHeader->Next()->previousSize = blockHeader->size;

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
        return nullptr;
    }

    AllocatorBlockHeader* currentBlock = reinterpret_cast<AllocatorBlockHeader*>(currentMemory->ptr);
    while (reinterpret_cast<unsigned char*>(currentBlock->Start()) + currentBlock->size < reinterpret_cast<unsigned char*>(pVoid))
    {
        currentBlock = currentBlock->Next();
    }

    return currentBlock;
}
#ifdef DEBUG
void GeneralPurposeAllocator::TestAllocate()
{
    // Тест 1: Выделение памяти и проверка корректности выделенной памяти, memory alignment и BlockHeader
    unsigned int size = 100;
    void* ptr = GeneralPurposeAllocator::Allocate(size);
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
    void* ptr1 = GeneralPurposeAllocator::Allocate(size1);
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
    void* ptr2 = GeneralPurposeAllocator::Allocate(size2);
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

GeneralPurposeAllocator::GeneralPurposeAllocator()
{
    Initialize();
}

GeneralPurposeAllocator::~GeneralPurposeAllocator()
{
    Shutdown();
}

void *GeneralPurposeAllocator::Allocate(size_t size)
{
    return s_Instance.AllocateMemory(size, 8);
}

void GeneralPurposeAllocator::Free(void *ptr)
{
    s_Instance.FreeMemory(ptr);
}

void GeneralPurposeAllocator::Shutdown()
{
    if (m_Memory != nullptr)
    {
        free(m_Memory);
        m_Memory = nullptr;
    }
}

#endif

AllocatorBlockHeader *AllocatorBlockHeader::Next()
{
    return (AllocatorBlockHeader*)((unsigned char*)this + sizeof(AllocatorBlockHeader) + size);
}
