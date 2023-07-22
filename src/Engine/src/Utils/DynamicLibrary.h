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

        void* GetFunction(const char* name);
    private:
        void* m_Handle;
    };
}
