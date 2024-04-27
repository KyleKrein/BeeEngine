//
// Created by Александр Лебедев on 20.11.2023.
//
#include "Utils/FileDialogs.h"
#include "SwiftToCpp.h"
namespace BeeEngine
{
    Path FileDialogs::OpenFile(FileDialogs::Filter filter)
    {
        auto filterStr = GetFilter(&filter);
        const char* path = openFileDialogMacOS(filterStr.c_str());
        if(path == nullptr)
            return {};
        return Path(path);
    }
    Path FileDialogs::SaveFile(FileDialogs::Filter filter)
    {
        auto filterStr = GetFilter(&filter);
        const char* path = saveFileDialogMacOS(filterStr.c_str());
        if(path == nullptr)
            return {};
        return Path(path);
    }
    Path FileDialogs::OpenFolder()
    {
        const char* path = openFolderDialogMacOS();
        if(path == nullptr)
            return {};
        return Path(path);
    }

    std::string FileDialogs::GetFilter(void *filter)
    {
        return std::string(((FileDialogs::Filter*)filter)->filter + 2);
    }
}