//
// Created by Aleksandr on 13.03.2024.
//
#include "Platform/Platform.h"
#include <windows.h>

namespace BeeEngine
{
    struct ClockData
    {
        float64_t Frequency = GetFrequency();
        LARGE_INTEGER StartTime = GetStartTime();

    private:
        float64_t GetFrequency()
        {
            LARGE_INTEGER frequency;
            ::QueryPerformanceFrequency(&frequency);
            return 1.0 / (float64_t)frequency.QuadPart;
        }
        LARGE_INTEGER GetStartTime()
        {
            LARGE_INTEGER counter;
            ::QueryPerformanceCounter(&counter);
            return counter;
        }
    };
    void* PlatformMemoryAllocate(size_t size, size_t alignment)
    {
        return _aligned_malloc(size, alignment);
    }
    void PlatformMemoryFree(void* ptr, size_t size, size_t alignment)
    {
        _aligned_free(ptr);
    }
    void* PlatformMemoryZero(void* ptr, size_t size)
    {
        return memset(ptr, 0, size);
    }
    void* PlatformMemoryCopy(void* dest, const void* src, size_t size)
    {
        return memcpy(dest, src, size);
    }
    void* PlatformMemorySet(void* dest, int32_t value, size_t size)
    {
        return memset(dest, value, size);
    }

    /*void PlatformConsolePrint(const char* message, Color4 foregroundC, Color4 backgroundC)
    {
        int32_t foreground = (int32_t)foregroundC;
        int32_t background = (int32_t)backgroundC;
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        HANDLE hConsoleError = GetStdHandle(STD_ERROR_HANDLE);
        size_t length = strlen(message);
        LPDWORD numberWritten = 0;
        SetConsoleTextAttribute(hConsole, (WORD)((background << 4) | foreground));
        OutputDebugStringA(message);
        WriteConsoleA(hConsole, message, length, numberWritten, nullptr);
        numberWritten = 0;
        WriteConsoleA(hConsoleError, message, length, numberWritten, nullptr);
        SetConsoleTextAttribute(hConsole, (WORD)(((int32_t)(Color4::Black) << 4) | (int32_t)(Color4::White)));
    }*/
    float64_t PlatformGetAbsoluteTime()
    {
        static ClockData data;
        LARGE_INTEGER counter;
        ::QueryPerformanceCounter(&counter);
        return (float64_t)(counter.QuadPart /* - data.StartTime.QuadPart*/) * data.Frequency;
    }
} // namespace BeeEngine