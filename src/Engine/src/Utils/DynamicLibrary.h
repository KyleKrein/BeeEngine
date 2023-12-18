//
// Created by alexl on 22.07.2023.
//

#pragma once
#include <filesystem>
#include "Core/Path.h"


namespace BeeEngine
{
    class DynamicLibrary
    {
    public:
        DynamicLibrary(const Path& path, const std::string& name);
        DynamicLibrary(const Path& path);
        ~DynamicLibrary();
        bool IsLoaded()
        {
            return m_Handle != nullptr;
        }
        void* GetFunction(const char* name);
        void Reload();
        void Unload();
    private:
        void* m_Handle;
        Path m_Path;
    };
}
