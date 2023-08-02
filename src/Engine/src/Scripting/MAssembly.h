//
// Created by alexl on 30.07.2023.
//

#pragma once
#include <filesystem>
#include "Core/TypeDefines.h"

extern "C"
{
    typedef struct _MonoAssembly MonoAssembly;
    typedef struct _MonoImage MonoImage;
}
namespace BeeEngine
{
    class MAssembly
    {
        friend class ScriptGlue;
    public:
        MAssembly() = default;
        MAssembly(const std::filesystem::path& path);
        ~MAssembly();
        MAssembly(const MAssembly&) = delete;
        MAssembly& operator=(const MAssembly&) = delete;
        MAssembly(MAssembly&& other) noexcept;
        MAssembly& operator=(MAssembly&&other) noexcept;
        std::vector<Ref<class MClass>>& GetClasses();

        void Reload();

    private:
        MonoAssembly* m_MonoAssembly = nullptr;
        MonoImage* m_MonoImage = nullptr;
        std::filesystem::path m_Path;
        std::vector<Ref<class MClass>> m_Classes;

        void LoadAssembly();
        void GetClassesFromAssembly();
        void UnloadAssembly();
    };
}
