//
// Created by Александр Лебедев on 16.05.2023.
//

#include "GeneralPurposeAllocator.h"
#include "MallocAndFree.h"

#define STANDART_MEMORY_SIZE 104857600//1024 * 1024 * 1024
#define BLOCK_HEADER_SIZE 9 //4 байта unsigned int - размер блока, 1 байт - bool false/true - занят/свободен, 4 байта unsigned int - размер блока до этого
#define BLOCK_SIZE_SIZE 4
#define MEMORY_ALIGNMENT 8

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


void *GeneralPurposeAllocator::Allocate(unsigned long long int size)
{
    if (!memory)
    {
        GeneralPurposeAllocator::Initialize(nullptr);
    }
    unsigned int* blockSizePtr = (unsigned int*)memory->ptr;
    bool* boolPtr = (bool*)(blockSizePtr + 1);
    unsigned int blockSize = *blockSizePtr;
    Node* memoryNode = memory;
    bool blockWasFound = false;
    while (!blockWasFound)
    {
        while (blockSize < size + BLOCK_HEADER_SIZE || !*boolPtr)
        {
            blockSizePtr = (unsigned int*)(boolPtr + 5 + blockSize);
            boolPtr = (bool*)(blockSizePtr +1);
            blockSize = *blockSizePtr;
            if (blockSize == 0)
            {
                break;
            }
        }
        if (blockSize)
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
        }
        memoryNode = memoryNode->next;
        blockSizePtr = (unsigned int*)memoryNode->ptr;
        boolPtr = (bool*)(blockSizePtr + 1);
        blockSize = *blockSizePtr;
    }

    if(blockSize/(size + BLOCK_HEADER_SIZE) > 1 && blockSize - (size + BLOCK_HEADER_SIZE * 2) >= 1)
    {
        unsigned int* newBlockSizePtr = (unsigned int*)(((unsigned char*)blockSizePtr) + 4 + 1 + 4 + size);
        *newBlockSizePtr = blockSize - size - BLOCK_HEADER_SIZE;
        *blockSizePtr = size;
        *((unsigned char*)newBlockSizePtr + 4) = 1;
        *(unsigned int*)((unsigned char*)newBlockSizePtr + 5) = size;
    }
    *boolPtr = false;
    return (void*)(((unsigned char*)blockSizePtr) + 9);
}

void GeneralPurposeAllocator::Free(void *ptr)
{
    unsigned int* blockSizePtr = (unsigned int*)((unsigned char*)ptr - 9);
    bool* boolPtr = (bool*)(blockSizePtr + 1);
    unsigned int* prevBlockSizePtr = (unsigned int*)(boolPtr + 1);

    *boolPtr = true;

    int blocks = CombineBlocks::No;


    if(*prevBlockSizePtr)
    {
        prevBlockSizePtr = (unsigned int*)(((unsigned char*)blockSizePtr)-*prevBlockSizePtr-9);
        bool* prevBoolPtr = (bool*)(prevBlockSizePtr+1);
        if(*prevBoolPtr)
        {
            *(unsigned int*)(prevBoolPtr-4)+=*blockSizePtr+5;
            blocks |= CombineBlocks::Previous;
        }
    }
    unsigned int* nextBlockSizePtr = (unsigned int*)(((unsigned char*)blockSizePtr) + *blockSizePtr + 5);
    if (*nextBlockSizePtr)
    {
        bool* nextBoolPtr = (bool*)(nextBlockSizePtr + 1);
        if (*nextBoolPtr)
        {
            *blockSizePtr += *nextBlockSizePtr + 5;
            blocks |= CombineBlocks::Next;
        }
    }

    if(blocks == CombineBlocks::No)
    {
        return;
    }

    if(blocks == 3)
    {
        unsigned int resultSize = *blockSizePtr + *prevBlockSizePtr + *nextBlockSizePtr + 18;
        *prevBlockSizePtr = resultSize;
        unsigned int* afterNextBlockSizePtr = (unsigned int*)(((unsigned char*)nextBlockSizePtr) + 5 + *((bool*)(nextBlockSizePtr)+5));
        *afterNextBlockSizePtr = resultSize;
        return;
    }
    if (blocks == 1)
    {
        unsigned int resultSize = *blockSizePtr + *prevBlockSizePtr + 9;
        *prevBlockSizePtr = resultSize;
        nextBlockSizePtr = (unsigned int*)(((unsigned char*)nextBlockSizePtr) + 5);
        *nextBlockSizePtr = resultSize;
        return;
    }
    if (blocks == 2)
    {
        unsigned int resultSize = *blockSizePtr + *nextBlockSizePtr + 9;
        *blockSizePtr = resultSize;
        nextBlockSizePtr = (unsigned int*)(((unsigned char*)nextBlockSizePtr) + 5 + *nextBlockSizePtr);
        *nextBlockSizePtr = resultSize;
        return;
    }
}

void GeneralPurposeAllocator::Initialize(void (*LogCallback)(const char *))
{
    LogCallbackFunc = LogCallback;
    findBlockSettings = FindBlock::First;

    memory = (Node*)bee_malloc(sizeof(Node));
    *memory = Node((unsigned char*)bee_malloc(STANDART_MEMORY_SIZE + BLOCK_HEADER_SIZE));
    if (!memory->ptr)
    {
        LogCallbackFunc("Unable to allocate memory");
    }

    *((unsigned int*)memory->ptr) = STANDART_MEMORY_SIZE;
    *(memory->ptr + 4) = 1;
    *(unsigned int*)(memory->ptr + 5) = 0;
}
