//
// Created by Aleksandr on 23.12.2023.
//

#include "NativeToManaged.h"

#include "Utils/DynamicLibrary.h"
#include "Core/String.h"
#include "MTypes.h"

#include <nethost.h>
#include <dotnethost/coreclr_delegates.h>
#include <dotnethost/hostfxr.h>

#include "Core/Logging/Log.h"
#if defined(WINDOWS)
#define CHAR_PREFIX L
#include "Platform/Windows/WindowsString.h"
#else
#define CHAR_PREFIX
#endif

namespace BeeEngine
{
    struct ArrayInfo
    {
        void* Ptr;
        uint64_t Length;
    };
    using CreateAssemblyContextFunction = uint64_t(CORECLR_DELEGATE_CALLTYPE *)(void* name, int32_t canBeUnloaded);
    using LoadAssemblyFromPathFunction = uint64_t(CORECLR_DELEGATE_CALLTYPE *)(uint64_t contextId, void* path);
    using GetClassesFromAssemblyFunction = ArrayInfo(CORECLR_DELEGATE_CALLTYPE *)(uint64_t contextId, uint64_t assemblyId);
    using FreeIntPtrFunction = void(CORECLR_DELEGATE_CALLTYPE *)(void* ptr);
    using GetClassNameFunction = void*(CORECLR_DELEGATE_CALLTYPE *)(uint64_t contextId, uint64_t assemblyId, uint64_t classId);
    using GetClassNamespaceFunction = void*(CORECLR_DELEGATE_CALLTYPE *)(uint64_t contextId, uint64_t assemblyId, uint64_t classId);
    using ClassIsValueTypeFunction = int32_t(CORECLR_DELEGATE_CALLTYPE *)(uint64_t contextId, uint64_t assemblyId, uint64_t classId);
    using ClassIsEnumFunction = int32_t(CORECLR_DELEGATE_CALLTYPE *)(uint64_t contextId, uint64_t assemblyId, uint64_t classId);
    using ClassIsDerivedFromFunction = int32_t(CORECLR_DELEGATE_CALLTYPE *)(uint64_t contextIdDerived, uint64_t assemblyIdDerived, uint64_t classIdDerived, uint64_t contextIdBase, uint64_t assemblyIdBase, uint64_t classIdBase);
    using MethodGetByNameFunction = uint64_t(CORECLR_DELEGATE_CALLTYPE *)(uint64_t contextId, uint64_t assemblyId, uint64_t classId, void* name, int32_t bindingFlags);
    using ClassGetFieldsFunction = ArrayInfo(CORECLR_DELEGATE_CALLTYPE *)(uint64_t contextId, uint64_t assemblyId, uint64_t classId, int32_t bindingFlags);
    using FieldGetNameFunction = void*(CORECLR_DELEGATE_CALLTYPE *)(uint64_t contextId, uint64_t assemblyId, uint64_t classId, uint64_t fieldId);
    using FieldGetTypeNameFunction = void*(CORECLR_DELEGATE_CALLTYPE *)(uint64_t contextId, uint64_t assemblyId, uint64_t classId, uint64_t fieldId);
    using FieldGetFlagsFunction = int32_t(CORECLR_DELEGATE_CALLTYPE *)(uint64_t contextId, uint64_t assemblyId, uint64_t classId, uint64_t fieldId);

    struct NativeToManaged::NativeToManagedData
    {
        NativeToManagedData(Path pathToNativeBridgeDll)
            :HostFxrLibrary(pathToNativeBridgeDll)
        {}
        DynamicLibrary HostFxrLibrary;
        hostfxr_initialize_for_dotnet_command_line_fn init_for_cmd_line_fptr = nullptr;
        hostfxr_initialize_for_runtime_config_fn init_for_config_fptr = nullptr;
        hostfxr_get_runtime_delegate_fn get_delegate_fptr = nullptr;
        hostfxr_run_app_fn run_app_fptr = nullptr;
        hostfxr_close_fn close_fptr = nullptr;

        load_assembly_and_get_function_pointer_fn load_assembly_and_get_function_pointer = nullptr;

        CreateAssemblyContextFunction CreateAssemblyContext = nullptr;
        LoadAssemblyFromPathFunction LoadAssemblyFromPath = nullptr;
        GetClassesFromAssemblyFunction GetClassesFromAssembly = nullptr;
        FreeIntPtrFunction FreeIntPtr = nullptr;
        GetClassNameFunction GetClassName = nullptr;
        GetClassNamespaceFunction GetClassNamespace = nullptr;
        ClassIsValueTypeFunction ClassIsValueType = nullptr;
        ClassIsEnumFunction ClassIsEnum = nullptr;
        ClassIsDerivedFromFunction ClassIsDerivedFrom = nullptr;
        MethodGetByNameFunction MethodGetByName = nullptr;
        ClassGetFieldsFunction ClassGetFields = nullptr;
        FieldGetNameFunction FieldGetName = nullptr;
        FieldGetTypeNameFunction FieldGetTypeName = nullptr;
        FieldGetFlagsFunction FieldGetFlags = nullptr;
    };
    NativeToManaged::NativeToManagedData* NativeToManaged::s_Data = nullptr;

    // Using the nethost library, discover the location of hostfxr and get exports
    bool NativeToManaged::init_hostfxr(const Path& assembly_path)
    {
        std::filesystem::path path;
        path = assembly_path.ToStdPath();
        get_hostfxr_parameters params = {sizeof(get_hostfxr_parameters), path.c_str(), nullptr};
        // Pre-allocate a large buffer for the path to hostfxr
        char_t buffer[256];
        size_t buffer_size = sizeof(buffer) / sizeof(char_t);
        const int rc = get_hostfxr_path(buffer, &buffer_size, &params);
        if (rc != 0)
            return false;

        // Load hostfxr and get desired exports
        s_Data = new NativeToManagedData(std::filesystem::path{buffer});
        auto& lib = s_Data->HostFxrLibrary;
        s_Data->init_for_cmd_line_fptr = (hostfxr_initialize_for_dotnet_command_line_fn)lib.GetFunction("hostfxr_initialize_for_dotnet_command_line");
        s_Data->init_for_config_fptr = (hostfxr_initialize_for_runtime_config_fn)lib.GetFunction("hostfxr_initialize_for_runtime_config");
        s_Data->get_delegate_fptr = (hostfxr_get_runtime_delegate_fn)lib.GetFunction("hostfxr_get_runtime_delegate");
        s_Data->run_app_fptr = (hostfxr_run_app_fn)lib.GetFunction("hostfxr_run_app");
        s_Data->close_fptr = (hostfxr_close_fn)lib.GetFunction("hostfxr_close");

        return (s_Data->init_for_config_fptr && s_Data->get_delegate_fptr && s_Data->close_fptr);
    }
    // Load and initialize .NET Core and get desired function pointer for scenario
    load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(const BeeEngine::Path& config_path, NativeToManaged::NativeToManagedData& data)
    {
        // Load .NET Core
        auto path = config_path.ToStdPath();
        void *load_assembly_and_get_function_pointer = nullptr;
        hostfxr_handle cxt = nullptr;
        int rc = data.init_for_config_fptr(path.c_str(), nullptr, &cxt);
        //int rc = data.init_for_cmd_line_fptr(0, nullptr, nullptr, &cxt);
        if (rc != 0 || cxt == nullptr)
        {
            BeeCoreError("Init failed: {0}", rc);
            //std::cerr << "Init failed: " << std::hex << std::showbase << rc << std::endl;
            data.close_fptr(cxt);
            return nullptr;
        }

        // Get the load assembly function pointer
        rc = data.get_delegate_fptr(
            cxt,
            hdt_load_assembly_and_get_function_pointer,
            &load_assembly_and_get_function_pointer);
        if (rc != 0 || load_assembly_and_get_function_pointer == nullptr)
            BeeCoreError("Get delegate failed: {0}", rc);
            //std::cerr << "Get delegate failed: " << std::hex << std::showbase << rc << std::endl;
        //std::cerr << "Get delegate failed: " << std::hex << std::showbase << rc << std::endl;


        data.close_fptr(cxt);
        return (load_assembly_and_get_function_pointer_fn)load_assembly_and_get_function_pointer;
    }
#define ObtainDelegate(name) \
{\
int rc = s_Data->load_assembly_and_get_function_pointer(\
path.c_str(),\
CHAR_PREFIX"BeeEngine.Internal.BridgeToNative, BeeEngine.NativeBridge",\
CHAR_PREFIX#name,\
UNMANAGEDCALLERSONLY_METHOD,\
nullptr,\
(void**)&s_Data->name);\
if(rc != 0)\
{\
BeeCoreError("Unable to obtain delegate for " #name "!");\
}\
}
    void NativeToManaged::Init(const Path& pathToNativeBridgeDll)
    {
        //
        // STEP 1: Load HostFxr and get exported hosting functions
        //
        if(!init_hostfxr(nullptr))
        {
            BeeCoreError("Unable to initialize .NET Host!");
        }
        //
        // STEP 2: Initialize and start the .NET Core runtime
        //
        const auto configPath = pathToNativeBridgeDll.GetParent() / "BeeEngine.NativeBridge.runtimeconfig.json";
        load_assembly_and_get_function_pointer_fn load_assembly_and_get_function_pointer = get_dotnet_load_assembly(configPath, *s_Data);
        if(!load_assembly_and_get_function_pointer)
        {
            BeeCoreError("Unable to load .NET Core runtime!");
        }
        //Obtain function pointers
        std::filesystem::path path = pathToNativeBridgeDll.ToStdPath();
        s_Data->load_assembly_and_get_function_pointer = load_assembly_and_get_function_pointer;
        ObtainDelegate(CreateAssemblyContext);
        ObtainDelegate(LoadAssemblyFromPath);
        ObtainDelegate(GetClassesFromAssembly);
        ObtainDelegate(FreeIntPtr);
        ObtainDelegate(GetClassName);
        ObtainDelegate(GetClassNamespace);
        ObtainDelegate(ClassIsValueType);
        ObtainDelegate(ClassIsEnum);
        ObtainDelegate(ClassIsDerivedFrom);
        ObtainDelegate(MethodGetByName);
        ObtainDelegate(ClassGetFields);
        ObtainDelegate(FieldGetName);
        ObtainDelegate(FieldGetTypeName);
        ObtainDelegate(FieldGetFlags);
    }
#undef ObtainDelegate
    ManagedAssemblyContextID NativeToManaged::CreateContext(const String& contextName, bool canBeUnloaded)
    {
        return s_Data->CreateAssemblyContext(const_cast<char*>(contextName.c_str()), canBeUnloaded?1:0);
    }

    void NativeToManaged::UnloadContext(ManagedAssemblyContextID contextID)
    {
    }

    void NativeToManaged::Shutdown()
    {
        delete s_Data;
    }

    ManagedAssemblyID NativeToManaged::LoadAssemblyFromPath(ManagedAssemblyContextID contextID, const Path& path)
    {
        return s_Data->LoadAssemblyFromPath(contextID, const_cast<char*>(path.AsCString()));
    }

    std::vector<ManagedClassID> NativeToManaged::GetClassesFromAssembly(ManagedAssemblyContextID contextID,
        ManagedAssemblyID assemblyID)
    {
        ArrayInfo info = s_Data->GetClassesFromAssembly(contextID, assemblyID);
        std::vector<ManagedClassID> result;
        result.resize(info.Length);
        memcpy(result.data(), info.Ptr, info.Length * sizeof(ManagedClassID));
        s_Data->FreeIntPtr(info.Ptr);
        return result;
    }
    String GetStringFromPtr(void* ptr)
    {
#if defined(WINDOWS)
        return Internal::WStringToUTF8(static_cast<wchar_t *>(ptr));
#else
        return ConvertUTF16ToUTF8(static_cast<char16_t*>(ptr));
#endif
    }

    String NativeToManaged::GetClassName(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId,
        ManagedClassID classID)
    {
        void* ptr = s_Data->GetClassName(contextID, assemblyId, classID);
        String name = GetStringFromPtr(ptr);
        s_Data->FreeIntPtr(ptr);
        return name;
    }

    String NativeToManaged::GetClassNamespace(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId,
        ManagedClassID classID)
    {
        void* ptr = s_Data->GetClassNamespace(contextID, assemblyId, classID);
        String name = GetStringFromPtr(ptr);
        s_Data->FreeIntPtr(ptr);
        return name;
    }

    bool NativeToManaged::ClassIsValueType(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId,
        ManagedClassID classID)
    {
        return s_Data->ClassIsValueType(contextID, assemblyId, classID) != 0;
    }

    bool NativeToManaged::ClassIsEnum(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId,
        ManagedClassID classID)
    {
        return s_Data->ClassIsEnum(contextID, assemblyId, classID) != 0;
    }

    bool NativeToManaged::ClassIsDerivedFrom(ManagedAssemblyContextID contextIdDerived,
        ManagedAssemblyID assemblyIdDerived, ManagedClassID classIdDerived, ManagedAssemblyContextID contextIdBase,
        ManagedAssemblyID assemblyIdBase, ManagedClassID classIdBase)
    {
        return s_Data->ClassIsDerivedFrom(contextIdDerived, assemblyIdDerived, classIdDerived, contextIdBase, assemblyIdBase, classIdBase) != 0;
    }

    ManagedMethodID NativeToManaged::MethodGetByName(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId,
        ManagedClassID classID, const String& methodName, ManagedBindingFlags flags)
    {
        return s_Data->MethodGetByName(contextID, assemblyId, classID, const_cast<char*>(methodName.c_str()), std::to_underlying(flags));
    }

    std::vector<ManagedFieldID> NativeToManaged::ClassGetFields(ManagedAssemblyContextID contextID,
        ManagedAssemblyID assemblyId, ManagedClassID classID, ManagedBindingFlags flags)
    {
        ArrayInfo info = s_Data->ClassGetFields(contextID, assemblyId, classID, std::to_underlying(flags));
        std::vector<ManagedFieldID> result;
        result.resize(info.Length);
        memcpy(result.data(), info.Ptr, info.Length * sizeof(ManagedFieldID));
        s_Data->FreeIntPtr(info.Ptr);
        return result;
    }

    String NativeToManaged::FieldGetName(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId,
        ManagedClassID classID, ManagedFieldID fieldID)
    {
        void* ptr = s_Data->FieldGetName(contextID, assemblyId, classID, fieldID);
        String name = GetStringFromPtr(ptr);
        s_Data->FreeIntPtr(ptr);
        return name;
    }

    String NativeToManaged::FieldGetTypeName(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId,
        ManagedClassID classID, ManagedFieldID fieldID)
    {
        void* ptr = s_Data->FieldGetTypeName(contextID, assemblyId, classID, fieldID);
        String name = GetStringFromPtr(ptr);
        s_Data->FreeIntPtr(ptr);
        return name;
    }

    MFieldFlags NativeToManaged::FieldGetFlags(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId,
        ManagedClassID classID, ManagedFieldID fieldID)
    {
        return static_cast<MFieldFlags>(s_Data->FieldGetFlags(contextID, assemblyId, classID, fieldID));
    }
}