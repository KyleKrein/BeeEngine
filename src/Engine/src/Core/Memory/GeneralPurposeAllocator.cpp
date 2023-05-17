//
// Created by Александр Лебедев on 16.05.2023.
//

#include "GeneralPurposeAllocator.h"
#include "MallocAndFree.h"

#define STANDART_MEMORY_SIZE 104857600//1024 * 1024 * 1024
#define BLOCK_HEADER_SIZE 9 //4 байта unsigned int - размер блока, 1 байт - bool false/true - занят/свободен, 4 байта unsigned int - размер блока до этого
#define BLOCK_SIZE_SIZE 4
#define MEMORY_ALIGNMENT 8

struct BlockHeader
{
    unsigned int size;
    bool isFree;
    unsigned int previousSize;
    BlockHeader(unsigned int size, bool isFree, unsigned int previousSize)
            :size(size), isFree(isFree), previousSize(previousSize) {}
    BlockHeader* Next()
    {
        return (BlockHeader*)((unsigned char*)this + BLOCK_HEADER_SIZE + size);
    }
    BlockHeader* Previous()
    {
        return (BlockHeader*)((unsigned char*)this - BLOCK_HEADER_SIZE - previousSize);
    }
    void* Start()
    {
        return (void*)((unsigned char*)(&size) + BLOCK_HEADER_SIZE);
    }
};

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


void *GeneralPurposeAllocator::Allocate(unsigned long long int size)
{
    if (!memory)
    {
        GeneralPurposeAllocator::Initialize(nullptr);
    }
    BlockHeader* blockHeader = (BlockHeader*)memory->ptr;
    Node* memoryNode = memory;
    bool blockWasFound = false;
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
        blockHeader->size = size;
        blockHeader->Next()->previousSize = size;
        blockHeader->Next()->size = blockHeader->Next()->size - size - BLOCK_HEADER_SIZE;
        blockHeader->Next()->isFree = true;
    }
    blockHeader->isFree = false;
    return blockHeader->Start();
}

void GeneralPurposeAllocator::Free(void *ptr)
{
    BlockHeader* blockHeader = (BlockHeader*)((unsigned char*)ptr - BLOCK_HEADER_SIZE);

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

    if(blocks == CombineBlocks::No)
    {
        return;
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
