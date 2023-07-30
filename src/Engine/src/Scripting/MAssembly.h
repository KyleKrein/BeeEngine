//
// Created by alexl on 30.07.2023.
//

#pragma once
#include <filesystem>

extern "C"
{
    typedef struct _MonoAssembly MonoAssembly;
    typedef struct _MonoImage MonoImage;
}
namespace BeeEngine
{
    class MAssembly
    {
    public:
        MAssembly(const std::filesystem::path& path);
        ~MAssembly();
        const std::vector<class MClass>& GetClasses();

        void Reload();

    private:
        MonoAssembly* m_MonoAssembly = nullptr;
        MonoImage* m_MonoImage = nullptr;
        std::filesystem::path m_Path;
        std::vector<class MClass> m_Classes;

        void LoadAssembly();
        void GetClassesFromAssembly();
        void UnloadAssembly();
    };
}
