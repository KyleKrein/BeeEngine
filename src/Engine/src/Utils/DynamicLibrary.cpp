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
#elif defined(MACOS)
        fullName = "lib" + name + ".dylib";
#elif defined(LINUX)
        fullName = "lib" + name + ".so";
#else
        #error "Unsupported platform"
#endif
        m_Path = path / fullName;
        Reload();
    }

    DynamicLibrary::~DynamicLibrary()
    {
        Unload();
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

    void DynamicLibrary::Reload()
    {
        Unload();
        BeeCoreTrace("Loading library {0}", m_Path.string());
#if defined(WINDOWS)
        m_Handle = LoadLibraryA(TEXT(m_Path.string().c_str()));
#elif defined(MACOS) || defined(LINUX)
        m_Handle = dlopen(m_Path.string().c_str(), RTLD_LAZY);
#endif
        if(!m_Handle)
            BeeCoreError("Failed to load library {0}", m_Path.string());
        else
            BeeCoreTrace("Library {0} was loaded successfully", m_Path.string());
    }

    void DynamicLibrary::Unload()
    {
        if(!m_Handle)
            return;
        BeeCoreTrace("Unloading library {0}", m_Path.string());
#if defined(WINDOWS)
        FreeLibrary((HMODULE)m_Handle);
#elif defined(MACOS) || defined(LINUX)
        dlclose(m_Handle);
#endif
    }
}
