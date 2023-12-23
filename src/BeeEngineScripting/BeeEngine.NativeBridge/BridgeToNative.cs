using System.Diagnostics;
using System.Reflection;
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
    
    private record struct TypeInfo(Type Type, Dictionary<ulong, MethodInfo> Methods);
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
                assemblyInfo.Types.Add(GetNewId(), new(type, new()));
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
                type.Methods.Add(array[i], methods[i]);
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
        return Marshal.StringToHGlobalUni(method.Name);
        error:
        Debug.Print("Unable to load method name from Class. Message: {0}", errorMessage);
        return 0;
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