//
// Created by Aleksandr on 23.12.2023.
//

#pragma once
#include "Core/Path.h"

namespace BeeEngine
{
    using ManagedAssemblyContextID = uint64_t;
    using ManagedAssemblyID = uint64_t;
    using ManagedClassID = uint64_t;
    using ManagedAssemblyMethodID = uint64_t;
    class NativeToManaged
    {
    public:
        struct NativeToManagedData;
        static void Init(const Path& pathToNativeBridgeDll);
        static ManagedAssemblyContextID CreateContext(const String& contextName, bool canBeUnloaded);
        static void UnloadContext(ManagedAssemblyContextID contextID);
        static void Shutdown();
        static ManagedAssemblyID LoadAssemblyFromPath(ManagedAssemblyContextID contextID, const Path& path);
        static std::vector<ManagedClassID> GetClassesFromAssembly(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyID);
        static String GetClassName(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId, ManagedClassID classID);
        static String GetClassNamespace(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId, ManagedClassID classID);
    private:
        static NativeToManagedData* s_Data;

        static bool init_hostfxr(const Path& assembly_path);
    };
}

