//
// Created by alexl on 22.07.2023.
//

#pragma once
#include <filesystem>


namespace BeeEngine
{
    class DynamicLibrary
    {
    public:
        DynamicLibrary(const std::filesystem::path& path, const std::string& name);
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
        std::filesystem::path m_Path;
    };
}
