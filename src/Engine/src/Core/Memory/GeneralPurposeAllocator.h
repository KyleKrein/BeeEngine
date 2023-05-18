//
// Created by Александр Лебедев on 16.05.2023.
//

#pragma once


class GeneralPurposeAllocator
{
public:
    static void Initialize(void(*LogCallback)(const char* message));
    static void* Allocate(unsigned long long int size, unsigned long long int alignment = 16);
    static void Free(void* ptr);
private:
    static void(*LogCallbackFunc)(const char* message);

    static void InitializeMemory(unsigned char *memory, unsigned long long size);

    static void* FindBlockHeader(void *pVoid);
};
