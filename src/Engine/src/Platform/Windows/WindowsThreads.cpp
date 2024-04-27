//
// Created by alexl on 26.11.2023.
//
#include <thread>
#include "Core/Numbers.h"
#include <windows.h>
namespace BeeEngine
{
    void ThreadSetAffinity(std::thread& thread, uint32_t core)
    {
        DWORD_PTR mask = 1ull << core;
        SetThreadAffinityMask(thread.native_handle(), mask);
    }
}
