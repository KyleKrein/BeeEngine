//
// Created by alexl on 22.07.2023.
//

#pragma once
#include "Core/Path.h"
#include <filesystem>

namespace BeeEngine
{
    class DynamicLibrary
    {
    public:
        DynamicLibrary(const Path& path, const std::string& name);
        DynamicLibrary(const Path& path);
        ~DynamicLibrary();
        bool IsLoaded() { return m_Handle != nullptr; }
        void* GetFunction(const char* name);
        void Reload();
        void Unload();

    private:
        void* m_Handle = nullptr;
        Path m_Path;
    };
} // namespace BeeEngine
