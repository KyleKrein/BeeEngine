//
// Created by Александр Лебедев on 16.05.2023.
//

#pragma once


class GeneralPurposeAllocator
{
public:
    static void Initialize(void(*LogCallback)(const char* message));
    static void* Allocate(unsigned long long size);
    static void Free(void* ptr);
private:
    static void(*LogCallbackFunc)(const char* message);
};
