using System.Diagnostics;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.Loader;
using System.Text;

namespace BeeEngine.Internal;

public static class AddressHelper
{
    private static object s_MutualObject;
    private static ObjectReinterpreter s_Reinterpreter;

    static AddressHelper()
    {
        s_MutualObject = new object();
        s_Reinterpreter = new ObjectReinterpreter();
        s_Reinterpreter.AsObject = new ObjectWrapper();
    }

    public static IntPtr GetAddress(object obj)
    {
        lock (AddressHelper.s_MutualObject)
        {
            AddressHelper.s_Reinterpreter.AsObject.Object = obj;
            IntPtr address = AddressHelper.s_Reinterpreter.AsIntPtr.Value;
            AddressHelper.s_Reinterpreter.AsObject.Object = null;
            return address;
        }
    }

    public static T GetInstance<T>(IntPtr address)
    {
        lock (AddressHelper.s_MutualObject)
        {
            AddressHelper.s_Reinterpreter.AsIntPtr.Value = address;
            T obj = (T)AddressHelper.s_Reinterpreter.AsObject.Object;
            AddressHelper.s_Reinterpreter.AsObject.Object = null;
            return obj;
        }
    }

    // I bet you thought C# was type-safe.
    [StructLayout(LayoutKind.Explicit)]
    private struct ObjectReinterpreter
    {
        [FieldOffset(0)] public ObjectWrapper AsObject;
        [FieldOffset(0)] public IntPtrWrapper AsIntPtr;
    }

    private class ObjectWrapper
    {
        public object Object;
    }

    private class IntPtrWrapper
    {
        public IntPtr Value;
    }
}

internal static class BridgeToNative
{
    private static ulong s_TempCurrentIdCounter = 1; //TODO: Replace with proper uuid generation

    static ulong GetNewId()
    {
        return s_TempCurrentIdCounter++;
    }
    
    private record struct MethodTypeInfo(MethodInfo Method, Delegate Delegate);
    //private record struct FieldTypeInfo(FieldInfo Field);
    private record struct TypeInfo(Type Type, Dictionary<ulong, MethodTypeInfo> Methods, Dictionary<ulong, FieldInfo> Fields);
    private record struct AssemblyInfo(Assembly Assembly, Dictionary<ulong, TypeInfo> Types);
    
    private record struct AssemblyLoadContextInfo(
        AssemblyLoadContext Context,
        Dictionary<ulong, AssemblyInfo> AssemblyInfo,
        bool CanBeUnloaded);
    
    private static Dictionary<ulong, AssemblyLoadContextInfo> s_LoadContexts = new();
    
    [UnmanagedCallersOnly]
    public static ulong CreateAssemblyContext(IntPtr namePtr, int canBeUnloaded)
    {
        string? name = Marshal.PtrToStringUTF8(namePtr);
        ulong newId = GetNewId();
        s_LoadContexts.Add(newId, new(new(name, canBeUnloaded != 0), new(), canBeUnloaded != 0));
        return newId;
    }
    [UnmanagedCallersOnly]
    public static ulong LoadAssemblyFromPath(ulong contextId, IntPtr pathPtr)
    {
        string? message = null;
        string? path = null;
        if (pathPtr == 0)
        {
            message = "Path pointer is null";
            goto error;
        }
        path = Marshal.PtrToStringUTF8(pathPtr);
        if (path == null)
        {
            message = "Unable to allocate memory for path string";
            goto error;
        }
        try
        {
            if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
            {
                message = "Context ID is not valid";
                goto error;
            }
            var assembly = contextInfo.Context.LoadFromAssemblyPath(path);
            ulong newId = GetNewId();
            contextInfo.AssemblyInfo.Add(newId, new(assembly, new()));
            return newId;
        }
        catch (Exception e)
        {
            message = e.Message;
        }
        error:
        Debug.Print("Unable to load Assembly from path {0}. Message: {1}", path, message);
        return 0;
    }

    /// <summary>
    /// ArrayInfo is a struct that contains a pointer to an array 
    /// of ulong and the length of the array.
    /// User MUST free the pointer returned by this method using FreeIntPtr later
    /// IMPORTANT: If this type is changed, the corresponding C++ type 
    /// in NativeToManaged.cpp must be changed as well
    /// </summary>
    /// <param name="Ptr"></param>
    /// <param name="Length"></param>
    [StructLayout(LayoutKind.Sequential)]
    public record struct ArrayInfo(IntPtr Ptr, ulong Length);
    //User MUST free the pointer returned by this method using FreeIntPtr later
    [UnmanagedCallersOnly]
    public static unsafe ArrayInfo GetClassesFromAssembly(ulong contextId, ulong assemblyId)
    {
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is invalid";
            goto error;
        }

        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is invalid";
            goto error;
        }

        if (assemblyInfo.Types.Count == 0)
        {
            var types = assemblyInfo.Assembly.GetTypes();
            foreach (var type in types)
            {
                assemblyInfo.Types.Add(GetNewId(), new(type, new(), new()));
            }
        }
        var array = assemblyInfo.Types.Keys.ToArray();
        var result = Marshal.AllocHGlobal(array.Length * sizeof(ulong));
        new Span<ulong>(array, 0, array.Length).CopyTo(new Span<ulong>((void*)result, array.Length));// Same as Marshal.Copy, but for ulong
        return new(result, (ulong)array.Length);
        error:
        Debug.Print("Unable to load classes from Assembly. Message: {0}", errorMessage);
        return new(0, 0);
    }
    [UnmanagedCallersOnly]
    public static void FreeIntPtr(IntPtr ptr)
    {
        Marshal.FreeHGlobal(ptr);
    }
    [UnmanagedCallersOnly]
    public static IntPtr GetClassName(ulong contextId, ulong assemblyId, ulong classId)
    {
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is invalid";
            goto error;
        }

        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is invalid";
            goto error;
        }

        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is invalid";
            goto error;
        }

        return Marshal.StringToHGlobalUni(type.Type.Name);
        
        error:
        Debug.Print("Unable to load class name from Assembly. Message: {0}", errorMessage);
        return 0;
    }
    [UnmanagedCallersOnly]
    public static IntPtr GetClassNamespace(ulong contextId, ulong assemblyId, ulong classId)
    {
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is invalid";
            goto error;
        }

        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is invalid";
            goto error;
        }

        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is invalid";
            goto error;
        }

        return Marshal.StringToHGlobalUni(type.Type.Namespace ?? "");
        
        error:
        Debug.Print("Unable to load class namespace from Assembly. Message: {0}", errorMessage);
        return 0;
    }
    [UnmanagedCallersOnly]
    public static IntPtr GetClassFullName(ulong contextId, ulong assemblyId, ulong classId)
    {
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is invalid";
            goto error;
        }

        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is invalid";
            goto error;
        }

        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is invalid";
            goto error;
        }

        return Marshal.StringToHGlobalUni(type.Type.FullName ?? "");
        error:
        Debug.Print("Unable to load class full name from Assembly. Message: {0}", errorMessage);
        return 0;
    }
    //User MUST free the pointer returned by this method using FreeIntPtr later
    [UnmanagedCallersOnly]
    public static unsafe ArrayInfo GetMethodsFromClass(ulong contextId, ulong assemblyId, ulong classId)
    {
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is invalid";
            goto error;
        }

        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is invalid";
            goto error;
        }

        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is invalid";
            goto error;
        }

        ulong[]? array = null;
        if (type.Methods.Count == 0)
        {
            var methods = type.Type.GetMethods();
            array = new ulong[methods.Length];
            for (int i = 0; i < methods.Length; i++)
            {
                array[i] = GetNewId();
                type.Methods.Add(array[i], new(methods[i], null));
            };
        }
        else
        {
            array = type.Methods.Keys.ToArray();
        }
        var result = Marshal.AllocHGlobal(array.Length * sizeof(ulong));
        new Span<ulong>(array, 0, array.Length).CopyTo(new Span<ulong>((void*)result, array.Length));// Same as Marshal.Copy, but for ulong
        return new(result, (ulong)array.Length);
        
        error:
        Debug.Print("Unable to load methods from Class. Message: {0}", errorMessage);
        return new(0, 0);
    }
    [UnmanagedCallersOnly]
    public static int ClassIsValueType(ulong contextId, ulong assemblyId, ulong classId)
    {
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is invalid";
            goto error;
        }

        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is invalid";
            goto error;
        }

        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is invalid";
            goto error;
        }

        return type.Type.IsValueType ? 1 : 0;
        
        error:
        Debug.Print("Unable to load info from Class. Message: {0}", errorMessage);
        return 0;
    }
    [UnmanagedCallersOnly]
    public static int ClassIsEnum(ulong contextId, ulong assemblyId, ulong classId)
    {
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is invalid";
            goto error;
        }

        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is invalid";
            goto error;
        }

        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is invalid";
            goto error;
        }

        return type.Type.IsEnum ? 1 : 0;
        
        error:
        Debug.Print("Unable to load info from Class. Message: {0}", errorMessage);
        return 0;
    }
    [UnmanagedCallersOnly]
    public static int ClassIsDerivedFrom(ulong contextIdDerived, ulong assemblyIdDerived, ulong classIdDerived, ulong contextIdBase, ulong assemblyIdBase, ulong classIdBase)
    {
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextIdDerived, out var contextInfoDerived))
        {
            errorMessage = "Derived Context ID is invalid";
            goto error;
        }

        if (!contextInfoDerived.AssemblyInfo.TryGetValue(assemblyIdDerived, out var assemblyInfoDerived))
        {
            errorMessage = "Derived Assembly ID is invalid";
            goto error;
        }

        if (!assemblyInfoDerived.Types.TryGetValue(classIdDerived, out var typeDerived))
        {
            errorMessage = "Derived Class ID is invalid";
            goto error;
        }
        
        if (!s_LoadContexts.TryGetValue(contextIdBase, out var contextInfoBase))
        {
            errorMessage = "Base Context ID is invalid";
            goto error;
        }

        if (!contextInfoBase.AssemblyInfo.TryGetValue(assemblyIdBase, out var assemblyInfoBase))
        {
            errorMessage = "Base Assembly ID is invalid";
            goto error;
        }

        if (!assemblyInfoBase.Types.TryGetValue(classIdBase, out var typeBase))
        {
            errorMessage = "Base Class ID is invalid";
            goto error;
        }

        return typeDerived.Type.IsSubclassOf(typeBase.Type) ? 1 : 0;
        
        error:
        Debug.Print("Unable to load info from Class. Message: {0}", errorMessage);
        return 0;
    }
    [UnmanagedCallersOnly]
    public static ulong MethodGetByName(ulong contextId, ulong assemblyId, ulong classId, IntPtr methodNamePtr, int bindingFlags)
    {
        string? errorMessage = null;
        string? methodName = null;
        if (methodNamePtr == IntPtr.Zero)
        {
            errorMessage = "Method name pointer is null";
            goto error;
        }
        methodName = Marshal.PtrToStringUTF8(methodNamePtr);
        if (methodName == null)
        {
            errorMessage = "Unable to allocate memory for method name string";
            goto error;
        }
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is invalid";
            goto error;
        }
        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is invalid";
            goto error;
        }
        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is invalid";
            goto error;
        }
        var method = type.Type.GetMethod(methodName, (BindingFlags)bindingFlags);
        if (method == null)
        {
            errorMessage = "Unable to find method with given name and binding flags";
            goto error;
        }
        ulong newId = GetNewId();
        type.Methods.Add(newId, new(method, null));
        return newId;
        error:
        Debug.Print("Unable to load method from Class. Message: {0}", errorMessage);
        return 0;
    }
    [UnmanagedCallersOnly]
    public static IntPtr MethodUnmanagedCallersOnlyGetFunctionPointer(ulong contextId, ulong assemblyId, ulong classId, ulong methodId)
    {
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is invalid";
            goto error;
        }
        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is invalid";
            goto error;
        }
        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is invalid";
            goto error;
        }
        if (!type.Methods.TryGetValue(methodId, out var method))
        {
            errorMessage = "Method ID is invalid";
            goto error;
        }
        return method.Method.MethodHandle.GetFunctionPointer();
        error:
        Debug.Print("Unable to load method from Class. Message: {0}", errorMessage);
        return IntPtr.Zero;
    }
    [UnmanagedCallersOnly]
    public static IntPtr MethodInstanceGetFunctionPointer(ulong contextId, ulong assemblyId, ulong classId, ulong methodId)
    {
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is invalid";
            goto error;
        }
        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is invalid";
            goto error;
        }
        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is invalid";
            goto error;
        }
        if (!type.Methods.TryGetValue(methodId, out var method))
        {
            errorMessage = "Method ID is invalid";
            goto error;
        }

        if (method.Delegate == null)
        {
            method.Delegate = method.Method.CreateDelegate<Delegate>();
        }
        return Marshal.GetFunctionPointerForDelegate(method.Delegate);
        error:
        Debug.Print("Unable to load method from Class. Message: {0}", errorMessage);
        return IntPtr.Zero;
    }
    [UnmanagedCallersOnly]
    public static IntPtr GetMethodName(ulong contextId, ulong assemblyId, ulong classId, ulong methodId)
    {
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is invalid";
            goto error;
        }

        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is invalid";
            goto error;
        }

        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is invalid";
            goto error;
        }

        if (!type.Methods.TryGetValue(methodId, out var method))
        {
            errorMessage = "Method ID is invalid";
            goto error;
        }
        return Marshal.StringToHGlobalUni(method.Method.Name);
        error:
        Debug.Print("Unable to load method name from Class. Message: {0}", errorMessage);
        return 0;
    }
    //User MUST free the pointer returned by this method using FreeIntPtr later
    [UnmanagedCallersOnly]
    public static unsafe ArrayInfo ClassGetFields(ulong contextId, ulong assemblyId, ulong classId, int bindingFlags)
    {
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is invalid";
            goto error;
        }

        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is invalid";
            goto error;
        }

        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is invalid";
            goto error;
        }
        //if(type.Fields.Count == 0)
        {
            var fields = type.Type.GetFields((BindingFlags)bindingFlags);
            foreach (var field in fields)
            {
                if(type.Fields.ContainsValue(field))
                {
                    continue;
                }
                type.Fields.Add(GetNewId(), field);
            }
        }
        var array = type.Fields.Keys.ToArray();
        var result = Marshal.AllocHGlobal(array.Length * sizeof(ulong));
        new Span<ulong>(array, 0, array.Length).CopyTo(new Span<ulong>((void*)result, array.Length));// Same as Marshal.Copy, but for ulong
        return new(result, (ulong)array.Length);
        error:
        Debug.Print("Unable to load fields from Class. Message: {0}", errorMessage);
        return new(0, 0);
    }
    //User MUST free the pointer returned by this method using FreeIntPtr later
    [UnmanagedCallersOnly]
    public static IntPtr FieldGetName(ulong contextId, ulong assemblyId, ulong classId, ulong fieldId)
    {
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is not valid";
            goto error;
        }
        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is not valid";
            goto error;
        }
        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is not valid";
            goto error;
        }
        if (!type.Fields.TryGetValue(fieldId, out var field))
        {
            errorMessage = "Field ID is not valid";
            goto error;
        }
        return Marshal.StringToHGlobalUni(field.Name);
        error:
        Debug.Print("Unable to load field name from Class. Message: {0}", errorMessage);
        return 0;
    }
    [UnmanagedCallersOnly]
    public static IntPtr FieldGetTypeName(ulong contextId, ulong assemblyId, ulong classId, ulong fieldId)
    {
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is not valid";
            goto error;
        }
        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is not valid";
            goto error;
        }
        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is not valid";
            goto error;
        }
        if (!type.Fields.TryGetValue(fieldId, out var field))
        {
            errorMessage = "Field ID is not valid";
            goto error;
        }
        return Marshal.StringToHGlobalUni(field.FieldType.Name);
        error:
        Debug.Print("Unable to load field type name from Class. Message: {0}", errorMessage);
        return 0;
    }
    [UnmanagedCallersOnly]
    public static int FieldGetFlags(ulong contextId, ulong assemblyId, ulong classId, ulong fieldId)
    {
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is not valid";
            goto error;
        }
        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is not valid";
            goto error;
        }
        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is not valid";
            goto error;
        }
        if (!type.Fields.TryGetValue(fieldId, out var field))
        {
            errorMessage = "Field ID is not valid";
            goto error;
        }
        return (int)field.Attributes;
        error:
        Debug.Print("Unable to load field type name from Class. Message: {0}", errorMessage);
        return 0;
    }
    [UnmanagedCallersOnly]
    public static void FieldSetValue(ulong contextId, ulong assemblyId, ulong classId, ulong fieldId, IntPtr instancePtr, IntPtr valuePtr)
    {
        string? errorMessage = null;
        object? instance = null;
        if (instancePtr != IntPtr.Zero)
        {
            instance = Marshal.PtrToStructure(instancePtr, typeof(object));
        }
        if (valuePtr == IntPtr.Zero)
        {
            errorMessage = "Value pointer is null";
            goto error;
        }
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is invalid";
            goto error;
        }
        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is invalid";
            goto error;
        }
        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is invalid";
            goto error;
        }
        if (!type.Fields.TryGetValue(fieldId, out var field))
        {
            errorMessage = "Field ID is invalid";
            goto error;
        }
        field.SetValue(instance, Marshal.PtrToStructure(valuePtr, field.FieldType));
        return;
        error:
        Debug.Print("Unable to set field from Class. Message: {0}", errorMessage);
    }
    [UnmanagedCallersOnly]
    public static unsafe IntPtr FieldGetValue(ulong contextId, ulong assemblyId, ulong classId, ulong fieldId, IntPtr instancePtr)
    {
        string? errorMessage = null;
        object? instance = null;
        if (instancePtr != IntPtr.Zero)
        {
            instance = Marshal.PtrToStructure(instancePtr, typeof(object));
        }
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is invalid";
            goto error;
        }
        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is invalid";
            goto error;
        }
        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is invalid";
            goto error;
        }
        if (!type.Fields.TryGetValue(fieldId, out var field))
        {
            errorMessage = "Field ID is invalid";
            goto error;
        }

        if (field.FieldType.IsValueType)
        {
            int size = Marshal.SizeOf(field.FieldType);
            IntPtr result = Marshal.AllocHGlobal(size);
            Marshal.StructureToPtr(field.GetValue(instance), result, false);
        }

        var value = field.GetValue(instance);
        if(value == null)
        {
            return IntPtr.Zero;
        }
        //GCHandle handle = GCHandle.Alloc(value, GCHandleType.Weak);
        //IntPtr resultPtr = GCHandle.ToIntPtr(handle);
        //handle.Free();
        IntPtr resultPtr = AddressHelper.GetAddress(value);
        return resultPtr;
        error:
        Debug.Print("Unable to get field from Class. Message: {0}", errorMessage);
        return IntPtr.Zero;
    }

    [UnmanagedCallersOnly]
    public static IntPtr FieldGetData(ulong contextId, ulong assemblyId, ulong classId, ulong fieldId,
        IntPtr instanceGcHandle)
    {
        GCHandle? handle = null;
        if(instanceGcHandle != IntPtr.Zero)
        {
            handle = GCHandle.FromIntPtr(instanceGcHandle);
        }
        object? obj = handle?.Target;
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var context))
        {
            errorMessage = "Context ID is invalid";
            goto error;
        }
        if (!context.AssemblyInfo.TryGetValue(assemblyId, out var assembly))
        {
            errorMessage = "Assembly ID is invalid";
            goto error;
        }
        if (!assembly.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is invalid";
            goto error;
        }
        if (!type.Fields.TryGetValue(fieldId, out var field))
        {
            errorMessage = "Field ID is invalid";
            goto error;
        }
        if (field.FieldType.IsValueType)
        {
            int size = Marshal.SizeOf(field.FieldType);
            IntPtr result = Marshal.AllocHGlobal(size);
            Marshal.StructureToPtr(field.GetValue(obj), result, false);
            return result;
        }
        GCHandle resultHandle = GCHandle.Alloc(field.GetValue(obj), GCHandleType.Normal);
        IntPtr resultPtr = GCHandle.ToIntPtr(resultHandle);
        return resultPtr;
        error:
        Debug.Print("Unable to get field data from Class. Message: {0}", errorMessage);
        return IntPtr.Zero;
    }
    [UnmanagedCallersOnly]
    public static void FieldSetData(ulong contextId, ulong assemblyId, ulong classId, ulong fieldId,
        IntPtr instanceGcHandle, IntPtr valuePtr)
    {
        GCHandle? handle = null;
        if (instanceGcHandle != IntPtr.Zero)
        {
            handle = GCHandle.FromIntPtr(instanceGcHandle);
        }
        object? obj = handle?.Target;
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var context))
        {
            errorMessage = "Context ID is invalid";
            goto error;
        }
        if (!context.AssemblyInfo.TryGetValue(assemblyId, out var assembly))
        {
            errorMessage = "Assembly ID is invalid";
            goto error;
        }
        if (!assembly.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is invalid";
            goto error;
        }
        if (!type.Fields.TryGetValue(fieldId, out var field))
        {
            errorMessage = "Field ID is invalid";
            goto error;
        }
        if(field.FieldType.IsValueType)
        {
            field.SetValue(obj, Marshal.PtrToStructure(valuePtr, field.FieldType));
            return;
        }
        GCHandle valueHandle = GCHandle.FromIntPtr(valuePtr);
        field.SetValue(obj, valueHandle.Target);
        return;
        error:
        Debug.Print("Unable to set field data from Class. Message: {0}", errorMessage);
    }
    [UnmanagedCallersOnly]
    public static IntPtr ObjectNew(ulong contextId, ulong assemblyId, ulong classId)
    {
        string? errorMessage = null;
        object? instance = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is not valid";
            goto error;
        }
        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is not valid";
            goto error;
        }
        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is not valid";
            goto error;
        }
        instance = Activator.CreateInstance(type.Type);
        //GCHandle handle = GCHandle.Alloc(instance, GCHandleType.Weak);
        //IntPtr resultPtr = GCHandle.ToIntPtr(handle);
        //handle.Free();
        IntPtr resultPtr = AddressHelper.GetAddress(instance);
        return resultPtr;
        error:
        Debug.Print("Unable to create object from Class. Message: {0}", errorMessage);
        return IntPtr.Zero;
    }
    //Handle types: 0 = weak, 1 = normal, 2 = pinned

    [UnmanagedCallersOnly]
    public static IntPtr ObjectNewGCHandle(ulong contextId, ulong assemblyId, ulong classId, int handleType)
    {
        string? errorMessage = null;
        object? instance = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is not valid";
            goto error;
        }
        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is not valid";
            goto error;
        }
        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is not valid";
            goto error;
        }
        instance = Activator.CreateInstance(type.Type);
        GCHandle handle = GCHandle.Alloc(instance, handleType == 0 ? GCHandleType.Weak : handleType == 1 ? GCHandleType.Normal : GCHandleType.Pinned);
        IntPtr resultPtr = GCHandle.ToIntPtr(handle);
        //handle.Free();
        return resultPtr;
        error:
        Debug.Print("Unable to create object from Class. Message: {0}", errorMessage);
        return IntPtr.Zero;
    }
    [UnmanagedCallersOnly]
    public static void ObjectFreeGCHandle(IntPtr handlePtr)
    {
        GCHandle handle = GCHandle.FromIntPtr(handlePtr);
        handle.Free();
    }
    [UnmanagedCallersOnly]
    public static void MemoryFree(IntPtr ptr)
    {
        Marshal.FreeHGlobal(ptr);
    }
    public static IntPtr ObjectGetAdressFromGCHandle(IntPtr handlePtr)
    {
        GCHandle handle = GCHandle.FromIntPtr(handlePtr);
        return AddressHelper.GetAddress(handle.Target);
    }

    [UnmanagedCallersOnly]
    public static unsafe IntPtr MethodInvoke(ulong contextId, ulong assemblyId, ulong classId, ulong methodId,
        IntPtr instanceGcHandlePtr, void** args)
    {
        GCHandle? instanceGcHandle = null;
        if(instanceGcHandlePtr != IntPtr.Zero)
        {
            instanceGcHandle = GCHandle.FromIntPtr(instanceGcHandlePtr);
        }
        object? instance = instanceGcHandle?.Target;
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is not valid";
            goto error;
        }
        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is not valid";
            goto error;
        }
        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is not valid";
            goto error;
        }
        if (!type.Methods.TryGetValue(methodId, out var method))
        {
            errorMessage = "Method ID is not valid";
            goto error;
        }

        var parameters = method.Method.GetParameters();
        var returnType = method.Method.ReturnType;
        object? result = null;
        Debug.WriteLine("MethodInvoke: {0} on instance {1} with parameters: {2}", method.Method.Name, instance is null ? "null" : instance.ToString(), parameters.Length);
        if (parameters.Length == 0)
        {
            try
            {
                result = method.Method.Invoke(instance, null);
            }
            catch (Exception e)
            {
                Debug.WriteLine("MethodInvoke Error: "+ e.Message);
                return IntPtr.Zero;
            }
            goto returnResult;
        }
        var argsArray = new object[parameters.Length];
        for (int i = 0; i < parameters.Length; i++)
        {
            Debug.Write("Parameter: ", i.ToString());
            var paramType = parameters[i].ParameterType;
            if(paramType.IsValueType)
                argsArray[i] = Marshal.PtrToStructure((IntPtr)args[i], paramType);
            else
            {
                GCHandle handle = GCHandle.FromIntPtr((IntPtr)args[i]);
                argsArray[i] = handle.Target;
            }
            Debug.WriteLine("{0}", (UIntPtr)args[i]);
        }
        try
        {
            result = method.Method.Invoke(instance, argsArray);
        }
        catch (Exception e)
        {
            Debug.WriteLine("MethodInvoke Error: "+ e.Message);
            return IntPtr.Zero;
        }
        returnResult:
        Debug.WriteLine("MethodInvoke Success");
        if (result == null)
        {
            return IntPtr.Zero;
        }

        IntPtr resultPtr;
        if (returnType.IsValueType)
        {
            int size = Marshal.SizeOf(returnType);
            resultPtr = Marshal.AllocHGlobal(size);
            Marshal.StructureToPtr(result, resultPtr, false);
            return resultPtr;
        }
        GCHandle resultHandle = GCHandle.Alloc(result, GCHandleType.Normal);
        resultPtr = GCHandle.ToIntPtr(resultHandle);
        return resultPtr;
        error:
        Debug.Print("Unable to invoke method from Class. Message: {0}", errorMessage);
        return IntPtr.Zero;
    }
    [UnmanagedCallersOnly]
    public static void UnmanagedMethodCreateDelegateAndSetToField(ulong contextId, ulong assemblyId, ulong classId, ulong fieldId, IntPtr gcHandlePtr, IntPtr functionPtr)
    {
        string? errorMessage = null;
        object? instance = null;
        if (gcHandlePtr != IntPtr.Zero)
        {
            instance = GCHandle.FromIntPtr(gcHandlePtr).Target;
        }
        if (functionPtr == IntPtr.Zero)
        {
            errorMessage = "Function pointer is null";
            goto error;
        }
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is not valid";
            goto error;
        }
        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is not valid";
            goto error;
        }
        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is not valid";
            goto error;
        }
        if (!type.Fields.TryGetValue(fieldId, out var field))
        {
            errorMessage = "Field ID is not valid";
            goto error;
        }
        field.SetValue(instance, /*Marshal.GetDelegateForFunctionPointer(*/functionPtr/*, field.FieldType)*/);
        Debug.Print("Delegate set to field. Field: {0}, Value {1}, Old {2}", field.Name, (nuint)field.GetValue(instance), (nuint)functionPtr);
        return;
        error:
        Debug.Print("Unable to set method from Class. Message: {0}", errorMessage);
    }

    [UnmanagedCallersOnly]
    public static void UnloadContext(ulong contextId)
    {
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            Debug.Print("Context ID is invalid");
            return;
        }
        if(!contextInfo.CanBeUnloaded)
        {
            Debug.Print("Context cannot be unloaded");
            return;
        }
        contextInfo.Context.Unload();
        s_LoadContexts.Remove(contextId);
    }

    [UnmanagedCallersOnly]
    public static IntPtr StringCreateManaged(IntPtr strPtr)
    {
        string? str = Marshal.PtrToStringUTF8(strPtr);
        GCHandle handle = GCHandle.Alloc(str, GCHandleType.Pinned);
        return GCHandle.ToIntPtr(handle);
    }
    /*[UnmanagedCallersOnly]
    public static void InvokeMethod(ulong contextId, ulong assemblyId, ulong classId, ulong methodId, object[] args)
    {
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is invalid";
            goto error;
        }

        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is invalid";
            goto error;
        }

        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is invalid";
            goto error;
        }

        if (!type.Methods.TryGetValue(methodId, out var method))
        {
            errorMessage = "Method ID is invalid";
            goto error;
        }

        method.Invoke(null, args);
        
        error:
        Debug.Print("Unable to invoke method from Class. Message: {0}", errorMessage);
    }
    [UnmanagedCallersOnly]
    public static Delegate GetNativeDelegateForMethod(ulong contextId, ulong assemblyId, ulong classId, ulong methodId)
    {
        string? errorMessage = null;
        if (!s_LoadContexts.TryGetValue(contextId, out var contextInfo))
        {
            errorMessage = "Context ID is invalid";
            goto error;
        }

        if (!contextInfo.AssemblyInfo.TryGetValue(assemblyId, out var assemblyInfo))
        {
            errorMessage = "Assembly ID is invalid";
            goto error;
        }

        if (!assemblyInfo.Types.TryGetValue(classId, out var type))
        {
            errorMessage = "Class ID is invalid";
            goto error;
        }

        if (!type.Methods.TryGetValue(methodId, out var method))
        {
            errorMessage = "Method ID is invalid";
            goto error;
        }

        return method.CreateDelegate<Delegate>();
        
        error:
        Debug.Print("Unable to get delegate for method from Class. Message: {0}", errorMessage);
        return null;
    }*/
}