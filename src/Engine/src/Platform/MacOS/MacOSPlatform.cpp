#include "Core/Logging/Log.h"
#include "Platform/Platform.h"
#include "Utils/Commands.h"
#include <cstdlib>
#include <mach/mach_time.h>

namespace BeeEngine
{
    struct TimeInfo
    {
        mach_timebase_info_data_t info = {0, 0};
        TimeInfo() { mach_timebase_info(&info); }
    };
    String RunCommand(const String& command)
    {
        char buffer[128];
        std::string result;

        // Открываем процесс для чтения
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
        if (!pipe)
        {
            BeeCoreError("popen() was unable to open process.");
        }

        while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr)
        {
            result += buffer;
        }

        return String{result};
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
        return nanoseconds / 1e9; // Конвертация наносекунд в секунды
    }
} // namespace BeeEngine