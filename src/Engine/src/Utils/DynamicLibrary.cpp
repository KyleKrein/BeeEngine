//
// Created by alexl on 22.07.2023.
//

#include "DynamicLibrary.h"
#include "Core/Logging/Log.h"

#if defined(WINDOWS)
    #include <Windows.h>
#elif defined(MACOS)
    #include <dlfcn.h>
#elif defined(LINUX)
    #include <dlfcn.h>
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
        static_assert(false, "Unsupported platform");
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
#elif defined(MACOS) || defined(LINUX)
        ptr = dlsym(m_Handle, name);
#endif
        if(ptr)
            return ptr;
#if defined(WINDOWS)
        BeeCoreError("Failed to get function {0}", name);
#elif defined(MACOS) || defined(LINUX)
        BeeCoreError("Failed to get function {0}: {1}", name, dlerror());
#endif
        return nullptr;
    }

    void DynamicLibrary::Reload()
    {
        Unload();
        BeeCoreTrace("Loading library {0}", m_Path.string());
        if(!std::filesystem::exists(m_Path))
        {
            BeeCoreError("Failed to load library {}: File doesn't exist", m_Path.string());
            return;
        }
#if defined(WINDOWS)
        m_Handle = LoadLibraryA(TEXT(m_Path.string().c_str()));
#elif defined(MACOS) || defined(LINUX)
        m_Handle = dlopen(m_Path.string().c_str(), RTLD_LAZY);
#endif
        if(!m_Handle)
        {
#if defined(WINDOWS)
            BeeCoreError("Failed to load library {0}", m_Path.string());
#elif defined(MACOS) || defined(LINUX)
            BeeCoreError("Failed to load library {0}: {1}", m_Path.string(), dlerror());
#endif
        }
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
        m_Handle = nullptr;
    }
}
