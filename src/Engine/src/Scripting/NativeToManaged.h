//
// Created by Aleksandr on 23.12.2023.
//

#pragma once
#include "Core/Path.h"

namespace BeeEngine
{
    enum MFieldFlags : int32_t;
}

namespace BeeEngine
{
    enum ManagedBindingFlags : int32_t;
}

namespace BeeEngine
{
    using ManagedAssemblyContextID = uint64_t;
    using ManagedAssemblyID = uint64_t;
    using ManagedClassID = uint64_t;
    using ManagedMethodID = uint64_t;
    using ManagedFieldID = uint64_t;
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
        static bool ClassIsValueType(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId, ManagedClassID classID);
        static bool ClassIsEnum(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId, ManagedClassID classID);
        static bool ClassIsDerivedFrom(ManagedAssemblyContextID contextIdDerived, ManagedAssemblyID assemblyIdDerived, ManagedClassID classIdDerived, ManagedAssemblyContextID contextIdBase, ManagedAssemblyID assemblyIdBase, ManagedClassID classIdBase);
        static ManagedMethodID MethodGetByName(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId, ManagedClassID classID, const String& methodName, ManagedBindingFlags flags);
        static std::vector<ManagedFieldID> ClassGetFields(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId, ManagedClassID classID, ManagedBindingFlags flags);
        static String FieldGetName(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId, ManagedClassID classID, ManagedFieldID fieldID);
        static String FieldGetTypeName(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId, ManagedClassID classID, ManagedFieldID fieldID);
        static MFieldFlags FieldGetFlags(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId, ManagedClassID classID, ManagedFieldID fieldID);
    private:
        static NativeToManagedData* s_Data;

        static bool init_hostfxr(const Path& assembly_path);
    };
}

