//
// Created by Александр Лебедев on 16.05.2023.
//

#pragma once

namespace BeeEngine::Internal
    {
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
        private:
            struct Node
            {
                unsigned char* ptr;
                Node* next;
                Node(unsigned char* pointer)
                        :ptr(pointer), next(nullptr) {}
            };
            Node* m_Memory = nullptr;
            //static void(*LogCallbackFunc)(const char* message);

            void InitializeMemory(unsigned char *memory, size_t size);

            void* FindBlockHeader(void *pVoid);

            static GeneralPurposeAllocator s_Instance;

            void CheckForUnfreedMemory();

            void PrintStatistics();
        };
    }