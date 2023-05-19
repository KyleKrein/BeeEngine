//
// Created by Александр Лебедев on 16.05.2023.
//

#pragma once
#include "corecrt.h"

struct AllocatorStatistics
{
    size_t allocatedMemory;
    size_t freedMemory;
    size_t totalAllocatedBlocks;
    size_t totalFreedBlocks;
    size_t allocatedBlocks;
    size_t freeBlocks;
    size_t totalMemoryPages;
    size_t blocksCombined;
    AllocatorStatistics()
            :allocatedMemory(0), freedMemory(0), totalAllocatedBlocks(0), totalFreedBlocks(0), allocatedBlocks(0), freeBlocks(0), totalMemoryPages(0), blocksCombined(0) {}

};

struct AllocatorBlockHeader
{
    unsigned int size;
    bool isFree;
    unsigned int previousSize;
    AllocatorBlockHeader(unsigned int size, bool isFree, unsigned int previousSize)
            :size(size), isFree(isFree), previousSize(previousSize) {}
    inline AllocatorBlockHeader* Next();
    inline AllocatorBlockHeader* Previous();
private:
    inline void align(int alignment, unsigned int sizeOfObject, void*& pVoid, unsigned int sizeOfBlock);
public:
    inline void* Start();

    inline void* StartWithoutAlignment();
};



class GeneralPurposeAllocator
{
public:
    GeneralPurposeAllocator();
    ~GeneralPurposeAllocator();
    void Initialize();
    static void* Allocate(size_t size);
    void* AllocateMemory(size_t size, unsigned int alignment);
    static void Free(void* ptr);
    void FreeMemory(void* ptr);
    void Shutdown();
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

    void InitializeMemory(unsigned char *memory, unsigned long long size);

    void* FindBlockHeader(void *pVoid);

    static void TestAllocate();

    static void TestALlocateAndFree();

    static thread_local GeneralPurposeAllocator s_Instance;
};
