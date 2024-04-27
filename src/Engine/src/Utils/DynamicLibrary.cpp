//
// Created by alexl on 22.07.2023.
//

#include "DynamicLibrary.h"
#include "Core/Logging/Log.h"
#include "Core/ResourceManager.h"
#include "FileSystem/File.h"

#if defined(WINDOWS)
    #include <Windows.h>
#elif defined(MACOS)
    #include <dlfcn.h>
#elif defined(LINUX)
    #include <dlfcn.h>
#endif
namespace BeeEngine
{

    DynamicLibrary::DynamicLibrary(const Path &path, const std::string &name)
    : m_Handle(nullptr)
    {
        std::string fullName;
#if defined(WINDOWS) || defined(MACOS) || defined(LINUX)
        fullName = ResourceManager::GetDynamicLibraryName(name);
#else
        static_assert(false, "Unsupported platform");
#endif
        m_Path = path / fullName;
        Reload();
    }

    DynamicLibrary::DynamicLibrary(const Path& path)
        :m_Path(path)
    {
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
        BeeCoreTrace("Loading library {0}", m_Path);
        if(!File::Exists(m_Path))
        {
            BeeCoreError("Failed to load library {}: File doesn't exist", m_Path);
            return;
        }
#if defined(WINDOWS)
        m_Handle = LoadLibraryW(TEXT(m_Path.ToStdPath().c_str()));
#elif defined(MACOS) || defined(LINUX)
        m_Handle = dlopen(m_Path.AsCString(), RTLD_LAZY);
#endif
        if(!m_Handle)
        {
#if defined(WINDOWS)
            BeeCoreError("Failed to load library {0}", m_Path);
#elif defined(MACOS) || defined(LINUX)
            BeeCoreError("Failed to load library {0}: {1}", m_Path.AsCString(), dlerror());
#endif
        }
        else
            BeeCoreTrace("Library {0} was loaded successfully", m_Path);
    }

    void DynamicLibrary::Unload()
    {
        if(!m_Handle)
            return;
        BeeCoreTrace("Unloading library {0}", m_Path);
#if defined(WINDOWS)
        FreeLibrary((HMODULE)m_Handle);
#elif defined(MACOS) || defined(LINUX)
        dlclose(m_Handle);
#endif
        m_Handle = nullptr;
    }
}
