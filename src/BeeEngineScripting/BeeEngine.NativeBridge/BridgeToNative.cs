using System.Diagnostics;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.Loader;

namespace BeeEngine.Internal;

internal static class BridgeToNative
{
    private static ulong s_TempCurrentIdCounter = 1; //TODO: Replace with proper uuid generation

    static ulong GetNewId()
    {
        return s_TempCurrentIdCounter++;
    }
    
    private record struct MethodTypeInfo(MethodInfo Method, Delegate Delegate);
    private record struct FieldTypeInfo(FieldInfo Field);
    private record struct TypeInfo(Type Type, Dictionary<ulong, MethodTypeInfo> Methods, Dictionary<ulong, FieldTypeInfo> Fields);
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
    public static unsafe ArrayInfo ClassGetFields(ulong contextId, ulong assemblyId, ulong classId)
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
        if(type.Fields.Count == 0)
        {
            var fields = type.Type.GetFields();
            foreach (var field in fields)
            {
                type.Fields.Add(GetNewId(), new(field));
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
    [UnmanagedCallersOnly]
    public static void ClassSetField(ulong contextId, ulong assemblyId, ulong classId, ulong fieldId, IntPtr instancePtr, IntPtr valuePtr)
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
        field.Field.SetValue(instance, Marshal.PtrToStructure(valuePtr, field.Field.FieldType));
        return;
        error:
        Debug.Print("Unable to set field from Class. Message: {0}", errorMessage);
    }
    [UnmanagedCallersOnly]
    public static unsafe IntPtr ClassGetField(ulong contextId, ulong assemblyId, ulong classId, ulong fieldId, IntPtr instancePtr)
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

        if (field.Field.FieldType.IsValueType)
        {
            int size = Marshal.SizeOf(field.Field.FieldType);
            IntPtr result = Marshal.AllocHGlobal(size);
            Marshal.StructureToPtr(field.Field.GetValue(instance), result, false);
        }

        var value = field.Field.GetValue(instance);
        if(value == null)
        {
            return IntPtr.Zero;
        }
        GCHandle handle = GCHandle.Alloc(value, GCHandleType.Weak);
        IntPtr resultPtr = GCHandle.ToIntPtr(handle);
        handle.Free();
        return resultPtr;
        error:
        Debug.Print("Unable to get field from Class. Message: {0}", errorMessage);
        return IntPtr.Zero;
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