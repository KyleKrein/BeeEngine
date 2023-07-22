//
// Created by alexl on 22.07.2023.
//

#include "DynamicLibrary.h"
#include "Core/Logging/Log.h"

#if defined(WINDOWS)
    #include <Windows.h>
#endif
namespace BeeEngine
{

    DynamicLibrary::DynamicLibrary(const std::filesystem::path &path, const std::string &name)
    : m_Handle(nullptr)
    {
        std::string fullName;
#if defined(WINDOWS)
        fullName = "lib" + name+ ".dll";
        auto pathStr = (path / fullName).string();
        BeeCoreTrace("Loading library {0}", pathStr);
        m_Handle = LoadLibraryA(TEXT(pathStr.c_str()));
#else
#endif
        if(!m_Handle)
            BeeCoreError("Failed to load library {0}", fullName);
        else
            BeeCoreTrace("Library {0} was loaded successfully", fullName);
    }

    DynamicLibrary::~DynamicLibrary()
    {
#if defined(WINDOWS)
        FreeLibrary((HMODULE)m_Handle);
#else
#endif
    }

    void *DynamicLibrary::GetFunction(const char *name)
    {
        void* ptr = nullptr;
#if defined(WINDOWS)
        ptr = reinterpret_cast<void *>(GetProcAddress((HINSTANCE) m_Handle, TEXT(name)));
#else
#endif
        if(ptr)
            return ptr;
        BeeCoreError("Failed to get function {0}", name);
        return nullptr;
    }
}
