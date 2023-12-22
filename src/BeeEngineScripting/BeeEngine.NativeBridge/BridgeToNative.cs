using System.Diagnostics;
using System.Reflection;
using System.Runtime.Loader;

namespace BeeEngine.Internal;

internal static class BridgeToNative
{
    private static ulong s_TempCurrentIdCounter = 1; //TODO: Replace with proper uuid generation

    static ulong GetNewId()
    {
        return s_TempCurrentIdCounter++;
    }
    private record struct AssemblyInfo(Assembly Assembly, Dictionary<ulong, Type> Types);

    private record struct AssemblyLoadContextInfo(
        AssemblyLoadContext Context,
        Dictionary<ulong, AssemblyInfo> AssemblyInfo,
        bool CanBeUnloaded);
    
    private static Dictionary<ulong, AssemblyLoadContextInfo> s_LoadContexts = new();
    internal static ulong CreateAssemblyContext(string name, bool canBeUnloaded)
    {
        ulong newId = GetNewId();
        s_LoadContexts.Add(newId, new(new(name, canBeUnloaded), new(), canBeUnloaded));
        return newId;
    }

    internal static ulong LoadAssemblyFromPath(ulong contextId, string path)
    {
        string? message = null;
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

    internal static ulong[] GetClassesFromAssembly(ulong contextId, ulong assemblyId)
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
                assemblyInfo.Types.Add(GetNewId(), type);
            }
        }

        return assemblyInfo.Types.Keys.ToArray();
        
        error:
        Debug.Print("Unable to load classes from Assembly. Message: {0}", errorMessage);
        return null;
    }
}