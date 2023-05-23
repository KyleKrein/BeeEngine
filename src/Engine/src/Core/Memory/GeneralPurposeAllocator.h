//
// Created by Александр Лебедев on 16.05.2023.
//

#pragma once

#include <cstdlib>
#include <atomic>

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
    AllocatorStatistics()
            : allocatedMemory(0), totalFreedMemory(0), totalAllocatedBlocks(0), totalFreedBlocks(0), allocatedBlocks(0), freeBlocks(0), totalMemoryPages(0), blocksCombined(0), totalAllocatedMemory(0) {}

};

struct AllocatorBlockHeader
{
    uint32_t size;
    uint64_t isFree;
    uint32_t previousSize;
    AllocatorBlockHeader(unsigned int size, bool isFree, unsigned int previousSize)
            :size(size), isFree(isFree), previousSize(previousSize) {}
    inline std::atomic<AllocatorBlockHeader>* Next(std::atomic<AllocatorBlockHeader>* ptr);
    inline std::atomic<AllocatorBlockHeader>* Previous(std::atomic<AllocatorBlockHeader>* ptr);
private:
    inline void align(int alignment, unsigned int sizeOfObject, void*& pVoid, size_t& sizeOfBlock);
public:
    inline void* Start(std::atomic<AllocatorBlockHeader>* ptr, unsigned int alignment);

    inline void* StartWithoutAlignment(std::atomic<AllocatorBlockHeader>* ptr);
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

    static void TestAllocate();

    static void TestALlocateAndFree();

    static GeneralPurposeAllocator s_Instance;

    void CheckForUnfreedMemory();

    void PrintStatistics();

    void MergeFreeBlocks(AllocatorBlockHeader *blockHeader, int blocks) const;
};
