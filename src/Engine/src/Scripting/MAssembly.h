//
// Created by alexl on 30.07.2023.
//

#pragma once
#include "Core/Path.h"
#include "Core/TypeDefines.h"
namespace BeeEngine
{
    class MAssembly
    {
        friend class ScriptGlue;
        friend class ScriptingEngine;

    public:
        MAssembly();
        MAssembly(uint64_t contextID, const Path& path, const Path& debugSymbolsPath);
        ~MAssembly();
        MAssembly(const MAssembly&) = delete;
        MAssembly& operator=(const MAssembly&) = delete;
        MAssembly(MAssembly&& other) noexcept;
        MAssembly& operator=(MAssembly&& other) noexcept;
        std::vector<Ref<class MClass>>& GetClasses();

    private:
        Path m_Path;
        Path m_DebugSymbolsPath;
        std::vector<Ref<class MClass>> m_Classes;
        uint64_t m_ContextID = 0;
        uint64_t m_AssemblyID = 0;

        void LoadAssembly();
        void GetClassesFromAssembly();
    };
} // namespace BeeEngine
