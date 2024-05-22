//
// Created by Aleksandr on 23.12.2023.
//
// clang-format off
#include "NativeToManaged.h"

#include "Gui/MessageBox.h"
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
    /**
     * @brief A struct to hold an array of uint64_t and its length
     * MUST be freed using FreeIntPtr
     * IMPORTANT: if this struct is changed, the corresponding struct 
     * in BridgeToNative.cs MUST also be changed
     */
    struct ArrayInfo
    {
        void* Ptr;
        uint64_t Length;
    };
    using SetupLoggerFunction = void(CORECLR_DELEGATE_CALLTYPE *)(void* info, void* warn, void* trace, void* error);
    using CreateAssemblyContextFunction = uint64_t(CORECLR_DELEGATE_CALLTYPE *)(void* name, int32_t canBeUnloaded);
    using LoadAssemblyFromPathFunction = uint64_t(CORECLR_DELEGATE_CALLTYPE *)(uint64_t contextId, void* path, void* debugSymbolsPath);
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
    using ObjectNewGCHandleFunction = void*(CORECLR_DELEGATE_CALLTYPE *)(uint64_t contextId, uint64_t assemblyId, uint64_t classId, int32_t type);
    using ObjectFreeGCHandleFunction = void(CORECLR_DELEGATE_CALLTYPE *)(void* handle);
    using FieldGetDataFunction = void*(CORECLR_DELEGATE_CALLTYPE *)(uint64_t contextId, uint64_t assemblyId, uint64_t classId, uint64_t fieldId, void* gcHandle);
    using FieldSetDataFunction = void(CORECLR_DELEGATE_CALLTYPE *)(uint64_t contextId, uint64_t assemblyId, uint64_t classId, uint64_t fieldId, void* gcHandle, void* data);
    using MethodInvokeFunction = void*(CORECLR_DELEGATE_CALLTYPE *)(uint64_t contextId, uint64_t assemblyId, uint64_t classId, uint64_t methodId, void* instanceGcHandle, void** args);
    using UnloadContextFunction = void(CORECLR_DELEGATE_CALLTYPE *)(uint64_t contextId);
    using StringCreateManagedFunction = void*(CORECLR_DELEGATE_CALLTYPE *)(void* str);
    using GetGCInfoFunction = NativeToManaged::GCInfo(CORECLR_DELEGATE_CALLTYPE *)();
    using GCCollectFunction = void(CORECLR_DELEGATE_CALLTYPE *)();

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
        ObjectNewGCHandleFunction ObjectNewGCHandle = nullptr;
        ObjectFreeGCHandleFunction ObjectFreeGCHandle = nullptr;
        FieldGetDataFunction FieldGetData = nullptr;
        FieldSetDataFunction FieldSetData = nullptr;
        MethodInvokeFunction MethodInvoke = nullptr;
        UnloadContextFunction UnloadContext = nullptr;
        StringCreateManagedFunction StringCreateManaged = nullptr;
        SetupLoggerFunction SetupLogger = nullptr;
        GetGCInfoFunction GetGCInfo = nullptr;
        GCCollectFunction GCCollect = nullptr;
    };
    NativeToManaged::NativeToManagedData* NativeToManaged::s_Data = nullptr;

    static void Log_Info(void* message)
    {
        BeeCoreInfo("C# : {}", NativeToManaged::StringGetFromManagedString(message));
    }
    static void Log_Warn(void* message)
    {
        BeeCoreWarn("C# : {}", NativeToManaged::StringGetFromManagedString(message));
    }
    static void Log_Error(void* message)
    {
        BeeCoreError("C# : {}", NativeToManaged::StringGetFromManagedString(message));
    }
    static void Log_Trace(void* message)
    {
        BeeCoreTrace("C# : {}", NativeToManaged::StringGetFromManagedString(message));
    }

    // Using the nethost library, discover the location of hostfxr and get exports
    bool NativeToManaged::init_hostfxr(const Path& assembly_path)
    {
        std::filesystem::path path;
        path = assembly_path.ToStdPath();
        get_hostfxr_parameters params = {sizeof(get_hostfxr_parameters), assembly_path.IsEmpty() ? nullptr : path.c_str(), nullptr};
        // Pre-allocate a large buffer for the path to hostfxr
        char_t buffer[256];
        size_t buffer_size = sizeof(buffer) / sizeof(char_t);
        const int rc = get_hostfxr_path(buffer, &buffer_size, &params);
        if (rc != 0)
        {
            BeeCoreError("get_hostfxr_path failed: {0}", rc);
            return false;
        }

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
    load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(const BeeEngine::Path& config_path, const NativeToManaged::NativeToManagedData& data)
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
        return reinterpret_cast<load_assembly_and_get_function_pointer_fn>(load_assembly_and_get_function_pointer);
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
            ShowMessageBox("Unable to initialize .NET Host!", "This program is unable to find .Net Runtime on your device. Please download and install it from dotnet.microsoft.com", MessageBoxType::Error);
            throw std::runtime_error("Unable to initialize .NET Host!");
        }
        //
        // STEP 2: Initialize and start the .NET Core runtime
        //
        const auto configPath = pathToNativeBridgeDll.GetParent() / "BeeEngine.NativeBridge.runtimeconfig.json";
        load_assembly_and_get_function_pointer_fn load_assembly_and_get_function_pointer = get_dotnet_load_assembly(configPath, *s_Data);
        if(!load_assembly_and_get_function_pointer)
        {
            BeeCoreError("Unable to load .NET Core runtime!");
            ShowMessageBox("Unable to load .NET Core runtime!", "This program is unable to find .Net Runtime on your device. Please download and install it from dotnet.microsoft.com", MessageBoxType::Error);
            throw std::runtime_error("Unable to initialize .NET Host!");
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
        ObtainDelegate(ObjectNewGCHandle);
        ObtainDelegate(ObjectFreeGCHandle);
        ObtainDelegate(FieldGetData);
        ObtainDelegate(FieldSetData);
        ObtainDelegate(MethodInvoke);
        ObtainDelegate(UnloadContext);
        ObtainDelegate(StringCreateManaged);
        ObtainDelegate(SetupLogger);
        ObtainDelegate(GetGCInfo);
        ObtainDelegate(GCCollect);
    }
#undef ObtainDelegate
    ManagedAssemblyContextID NativeToManaged::CreateContext(const String& contextName, bool canBeUnloaded)
    {
        return s_Data->CreateAssemblyContext(const_cast<char*>(contextName.c_str()), canBeUnloaded?1:0);
    }

    void NativeToManaged::UnloadContext(ManagedAssemblyContextID contextID)
    {
        s_Data->UnloadContext(contextID);
    }

    void NativeToManaged::Shutdown()
    {
        delete s_Data;
    }

    ManagedAssemblyID NativeToManaged::LoadAssemblyFromPath(ManagedAssemblyContextID contextID, const Path& path, const std::optional<Path>& debugSymbolsPath)
    {
        return s_Data->LoadAssemblyFromPath(contextID, const_cast<char*>(path.AsCString()), debugSymbolsPath.has_value() ? const_cast<char*>(debugSymbolsPath->AsCString()) : nullptr);
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
        return StringGetFromManagedString(ptr);
    }

    String NativeToManaged::GetClassNamespace(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId,
        ManagedClassID classID)
    {
        void* ptr = s_Data->GetClassNamespace(contextID, assemblyId, classID);
        return StringGetFromManagedString(ptr);
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
        return StringGetFromManagedString(ptr);
    }

    String NativeToManaged::FieldGetTypeName(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId,
        ManagedClassID classID, ManagedFieldID fieldID)
    {
        void* ptr = s_Data->FieldGetTypeName(contextID, assemblyId, classID, fieldID);
        return StringGetFromManagedString(ptr);
    }

    MFieldFlags NativeToManaged::FieldGetFlags(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId,
        ManagedClassID classID, ManagedFieldID fieldID)
    {
        return static_cast<MFieldFlags>(s_Data->FieldGetFlags(contextID, assemblyId, classID, fieldID));
    }

    GCHandle NativeToManaged::ObjectNewGCHandle(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId,
        ManagedClassID classID, GCHandleType type)
    {
        return s_Data->ObjectNewGCHandle(contextID, assemblyId, classID, std::to_underlying(type));
    }

    void NativeToManaged::ObjectFreeGCHandle(GCHandle handle)
    {
        s_Data->ObjectFreeGCHandle(handle);
    }

    void* NativeToManaged::FieldGetData(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId,
        ManagedClassID classID, ManagedFieldID fieldID, GCHandle objectHandle)
    {
        return s_Data->FieldGetData(contextID, assemblyId, classID, fieldID, objectHandle);
    }

    void NativeToManaged::FieldSetData(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId,
        ManagedClassID classID, ManagedFieldID fieldID, GCHandle objectHandle, void* data)
    {
        s_Data->FieldSetData(contextID, assemblyId, classID, fieldID, objectHandle, data);
    }

    void* NativeToManaged::MethodInvoke(ManagedAssemblyContextID contextID, ManagedAssemblyID assemblyId,
        ManagedClassID classID, ManagedMethodID methodID, GCHandle objectHandle, void** args)
    {
        return s_Data->MethodInvoke(contextID, assemblyId, classID, methodID, objectHandle, args);
    }

    String NativeToManaged::StringGetFromManagedString(void* managedString)
    {
        String result = GetStringFromPtr(managedString);
        s_Data->FreeIntPtr(managedString);
        return result;
    }
    GCHandle NativeToManaged::StringCreateManaged(const String &string)
    {
        return s_Data->StringCreateManaged((void*)(string.c_str()));
    }
    void NativeToManaged::FreeIntPtr(void* ptr)
    {
        s_Data->FreeIntPtr(ptr);
    }
    void NativeToManaged::SetupLogger()
    {
        s_Data->SetupLogger((void*)Log_Info, (void*)Log_Warn, (void*)Log_Trace, (void*)Log_Error);
    }

    NativeToManaged::GCInfo NativeToManaged::GetGCInfo()
    {
        return s_Data->GetGCInfo();
    }
    void NativeToManaged::GCCollect()
    {
        s_Data->GCCollect();
    }
}
