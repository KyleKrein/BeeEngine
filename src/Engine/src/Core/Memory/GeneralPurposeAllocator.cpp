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

#define STANDART_MEMORY_SIZE 104857600//1024 * 1024 * 1024
//#define BLOCK_HEADER_SIZE 9 //4 байта unsigned int - размер блока, 1 байт - bool false/true - занят/свободен, 4 байта unsigned int - размер блока до этого
#define BLOCK_SIZE_SIZE 4
#define MEMORY_ALIGNMENT 8

struct BlockHeader
{
    unsigned int size;
    bool isFree;
    unsigned int previousSize;
    BlockHeader(unsigned int size, bool isFree, unsigned int previousSize)
            :size(size), isFree(isFree), previousSize(previousSize) {}
    inline BlockHeader* Next()
    {
        return (BlockHeader*)((unsigned char*)this + sizeof(BlockHeader) + size);
    }
    inline BlockHeader* Previous()
    {
        return (BlockHeader*)((unsigned char*)this - sizeof(BlockHeader) - previousSize);
    }
private:
    inline void align(int alignment, unsigned int sizeOfObject, void*& pVoid, unsigned int sizeOfBlock)
    {
        pVoid = (void*)(((uintptr_t)(pVoid) + alignment - 1) & ~(alignment - 1));
    }
public:
    inline void* Start()
    {
        void* aligned_ptr = (unsigned char*)(&this->size) + sizeof(BlockHeader);//(void*)(((uintptr_t)(this) + sizeof(BlockHeader) + MEMORY_ALIGNMENT - 1) & ~(MEMORY_ALIGNMENT));
        size_t S = size;
        std::align(MEMORY_ALIGNMENT, size-MEMORY_ALIGNMENT+1, aligned_ptr, S);
        BeeCoreAssert(alignof(aligned_ptr) == MEMORY_ALIGNMENT, "Wrong alignment");
        return aligned_ptr; //(void*)((unsigned char*)(&size) + BLOCK_HEADER_SIZE);
    }
};

#define BLOCK_HEADER_SIZE sizeof(BlockHeader)

enum class FindBlock: unsigned char
{
    First = 0,
    Best = 1,
};
enum CombineBlocks
{
    No = 0,
    Previous = 1,
    Next = 2,
    Both = 3,
};

struct Node
{
    unsigned char* ptr;
    Node* next;
    Node(unsigned char* pointer)
    :ptr(pointer), next(nullptr) {}
};
Node* memory = nullptr;

FindBlock findBlockSettings = FindBlock::First;



void (*GeneralPurposeAllocator::LogCallbackFunc)(const char *) = nullptr;


void *GeneralPurposeAllocator::Allocate(unsigned long long int size, unsigned long long int  alignment)
{
    if (!memory)
    {
        GeneralPurposeAllocator::Initialize(nullptr);
    }

    if(size == 0)
    {
        return nullptr;
    }

    BlockHeader* blockHeader = (BlockHeader*)memory->ptr;
    Node* memoryNode = memory;
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
                if (LogCallbackFunc)
                {
                    LogCallbackFunc("GeneralPurposeAllocator: Not enough memory");
                }
                return nullptr;
            }
            memoryNode->next = (Node*)bee_malloc(sizeof(Node));
            *memoryNode->next = Node(newMemory);
            InitializeMemory(newMemory, STANDART_MEMORY_SIZE);
        }
        memoryNode = memoryNode->next;
    }

    if(blockHeader->size/(size + BLOCK_HEADER_SIZE) > 1 && blockHeader->size - (size + BLOCK_HEADER_SIZE * 2) >= 1)
    {
        BlockHeader* nextNextBlockHeader = blockHeader->Next();
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

    return blockHeader->Start();
}

void GeneralPurposeAllocator::Free(void *ptr)
{

    BlockHeader* blockHeader = (BlockHeader*)FindBlockHeader(ptr);//(BlockHeader*)ptr - 1;//(BlockHeader*)((unsigned char*)*(uintptr_t*)((unsigned char*)(ptr) - sizeof(void*))- BLOCK_HEADER_SIZE);

    if (blockHeader->isFree)
    {
        if (LogCallbackFunc)
        {
            LogCallbackFunc("GeneralPurposeAllocator: Double free");
        }
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

    unsigned int resultSize;

    switch ((CombineBlocks)blocks)
    {
        case CombineBlocks::No:
            return;
        case CombineBlocks::Previous:
            resultSize = blockHeader->size + blockHeader->previousSize + BLOCK_HEADER_SIZE;
            blockHeader->Previous()->size = resultSize;
            blockHeader->Next()->previousSize = resultSize;
            break;
        case CombineBlocks::Next:
            resultSize = blockHeader->size + blockHeader->Next()->size + BLOCK_HEADER_SIZE;
            blockHeader->size = resultSize;
            blockHeader->Next()->Next()->previousSize = resultSize;
            break;
        case CombineBlocks::Both:
            resultSize = blockHeader->size + blockHeader->previousSize + blockHeader->Next()->size + BLOCK_HEADER_SIZE * 2;
            blockHeader->Previous()->size = resultSize;
            blockHeader->Next()->Next()->previousSize = resultSize;
            break;
    }
}

void GeneralPurposeAllocator::Initialize(void (*LogCallback)(const char *))
{
    LogCallbackFunc = LogCallback;
    findBlockSettings = FindBlock::First;

    memory = (Node*)bee_malloc(sizeof(Node));
    *memory = Node((unsigned char*)bee_malloc(STANDART_MEMORY_SIZE));
    if (!memory->ptr)
    {
        LogCallbackFunc("Unable to allocate memory");
    }

    InitializeMemory(memory->ptr, STANDART_MEMORY_SIZE);

#ifdef DEBUG
    TestAllocate();
    TestALlocateAndFree();
#endif
}

void GeneralPurposeAllocator::InitializeMemory(unsigned char *memory, unsigned long long size)
{
    BlockHeader* blockHeader = (BlockHeader*)memory;
    blockHeader->size = size - BLOCK_HEADER_SIZE * 2;
    blockHeader->isFree = true;
    blockHeader->previousSize = 0;
    blockHeader->Next()->size = 0;
    blockHeader->Next()->isFree = false;
    blockHeader->Next()->previousSize = blockHeader->size;
}

void* GeneralPurposeAllocator::FindBlockHeader(void *pVoid)
{
    Node* currentMemory = memory;
    while (!(currentMemory == nullptr || currentMemory->ptr <= pVoid && pVoid < currentMemory->ptr + STANDART_MEMORY_SIZE))
    {
        currentMemory = currentMemory->next;
    }

    if (!currentMemory)
    {
        LogCallbackFunc("Unable to free memory: memory was allocated with different allocator");
        return nullptr;
    }

    BlockHeader* currentBlock = reinterpret_cast<BlockHeader*>(currentMemory->ptr);
    while (reinterpret_cast<unsigned char*>(currentBlock->Start()) + currentBlock->size < reinterpret_cast<unsigned char*>(pVoid))
    {
        currentBlock = currentBlock->Next();
    }

    return currentBlock;
}

void GeneralPurposeAllocator::TestAllocate()
{
    // Тест 1: Выделение памяти и проверка корректности выделенной памяти, memory alignment и BlockHeader
    unsigned int size = 100;
    void* ptr = GeneralPurposeAllocator::Allocate(size, 8);
    if (ptr != nullptr)
    {
        // Проверка корректности выделенной памяти
        BlockHeader* blockHeader = static_cast<BlockHeader*>(GeneralPurposeAllocator::FindBlockHeader(ptr));
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
        BlockHeader* blockHeader1 = static_cast<BlockHeader*>(GeneralPurposeAllocator::FindBlockHeader(ptr1));
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
        BlockHeader* blockHeader2 = static_cast<BlockHeader*>(GeneralPurposeAllocator::FindBlockHeader(ptr2));
        if (blockHeader2 != nullptr)
        {
            std::cout << "Test 2: Memory 2 allocation successful. Allocated size: " << blockHeader2->size << std::endl;
        }
        else
        {
            std::cout << "Test 2: Memory 2 allocation failed. Invalid block header." << std::endl;
            return;
        }
        BlockHeader* blockHeader1 = static_cast<BlockHeader*>(GeneralPurposeAllocator::FindBlockHeader(ptr1));
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
        /*
        if(!blockHeader2->Next()->isFree && blockHeader2->Next()->size == 0 && blockHeader2->Next()->previousSize == blockHeader2->size)
        {
            std::cout << "Test 2: Memory 2 block is correct." << std::endl;
        }
        else
        {
            std::cout << "Test 2: Memory 2 block is incorrect." << std::endl;
            return;
        }
         */
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
    BlockHeader* blockHeader1 = static_cast<BlockHeader*>(GeneralPurposeAllocator::FindBlockHeader(ptr1));
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

