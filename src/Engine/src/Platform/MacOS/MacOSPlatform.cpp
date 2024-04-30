#include "Utils/Commands.h"
#include <cstdlib>
#include "Core/Logging/Log.h"
#include "Platform/Platform.h"
#include <mach/mach_time.h>

namespace BeeEngine
{
    struct TimeInfo
    {
        mach_timebase_info_data_t info = {0, 0};
        TimeInfo()
        {
            mach_timebase_info(&info);
        }
    };
    void RunCommand(const String& command)
    {
        int result = system(command.c_str());
        if(result != 0)
        {
            BeeCoreError("Command failed: {0}. Error code: {1}", command, result);
        }
    }
    void* PlatformMemoryAllocate(size_t size, size_t alignment)
    {
        return aligned_alloc(alignment, size);
    }
    void PlatformMemoryFree(void* ptr, size_t size, size_t alignment)
    {
        free(ptr);
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
    float64_t PlatformGetAbsoluteTime()
    {
        static TimeInfo info;
        uint64_t time = mach_absolute_time();
        float64_t nanoseconds = (float64_t)time * (float64_t)info.info.numer / (float64_t)info.info.denom;
        return nanoseconds / 1e9;  // Конвертация наносекунд в секунды
    }
}