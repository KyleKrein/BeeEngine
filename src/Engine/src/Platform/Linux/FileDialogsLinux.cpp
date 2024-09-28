//
// Created by alexl on 06.06.2023.
//
#include "Utils/FileDialogs.h"
namespace BeeEngine
{
    Path FileDialogs::OpenFile(FileDialogs::Filter filter)
    {
        auto filterStr = GetFilter(&filter);
        const char* path = nullptr; // TODO: implement
        if (path == nullptr)
            return {};
        return Path(path);
    }
    Path FileDialogs::SaveFile(FileDialogs::Filter filter)
    {
        auto filterStr = GetFilter(&filter);
        const char* path = nullptr; // TODO: implement
        if (path == nullptr)
            return {};
        return Path(path);
    }
    Path FileDialogs::OpenFolder()
    {
        const char* path = nullptr; // TODO: implement
        if (path == nullptr)
            return {};
        return Path(path);
    }

    String FileDialogs::GetFilter(void* filter)
    {
        return String(((FileDialogs::Filter*)filter)->filter + 2);
    }
} // namespace BeeEngine