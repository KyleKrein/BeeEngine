using BeeEngine.Math;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace BeeEngine.Internal
{
    internal unsafe static class InternalCalls
    {
        private static delegate* unmanaged<IntPtr, void> s_Log_Warn = null;
        private static delegate* unmanaged<IntPtr, void> s_Log_Error = null;
        private static delegate* unmanaged<IntPtr, void> s_Log_Info = null;
        private static delegate* unmanaged<IntPtr, void> s_Log_Trace = null;
        private static delegate* unmanaged<IntPtr, ulong> s_Entity_FindEntityByName = null;
        private static delegate* unmanaged<ulong, ulong> s_Entity_GetParent = null;
        private static delegate* unmanaged<ulong, ulong, void> s_Entity_SetParent = null;
        private static delegate* unmanaged<ulong, ulong, ulong> s_Entity_GetNextChild = null;
        private static delegate* unmanaged<ulong, ulong, int> s_Entity_HasChild = null;
        private static delegate* unmanaged<ulong, ulong, void> s_Entity_AddChild = null;
        private static delegate* unmanaged<ulong, ulong, void> s_Entity_RemoveChild = null;
        private static delegate* unmanaged<ulong, IntPtr> s_Entity_GetName = null;
        private static delegate* unmanaged<ulong, IntPtr, void> s_Entity_SetName = null;
        private static delegate* unmanaged<ulong, IntPtr> s_TextRendererComponent_GetText = null;
        private static delegate* unmanaged<ulong, IntPtr, void> s_TextRendererComponent_SetText = null;
        private static delegate* unmanaged<ulong, void> s_Entity_Destroy = null;
        private static delegate* unmanaged<ulong, ulong> s_Entity_Duplicate = null;
        private static delegate* unmanaged<void*, ulong, ulong> s_Entity_InstantiatePrefab = null;
        private static delegate* unmanaged<int, int> s_Input_IsKeyDown = null;
        private static delegate* unmanaged<int, int> s_Input_IsMouseButtonDown = null;
        private static delegate* unmanaged<void*, void> s_Input_GetMousePosition = null;
        private static delegate* unmanaged<ulong, void*, void> s_Input_GetMousePositionInWorldSpace = null;
        private static delegate* unmanaged<void*, void> s_Asset_Load = null;
        private static delegate* unmanaged<void*, void> s_Asset_Unload = null;
        private static delegate* unmanaged<void*, int> s_Asset_IsValid = null;
        private static delegate* unmanaged<void*, int> s_Asset_IsLoaded = null;
        private static delegate* unmanaged<void*, void*, ulong> s_Physics2D_CastRay = null;
        private static delegate* unmanaged<IntPtr> s_Locale_GetLocale = null;
        private static delegate* unmanaged<IntPtr, void> s_Locale_SetLocale = null;
        private static delegate* unmanaged<IntPtr, IntPtr> s_Locale_TranslateStatic = null;
        private static delegate* unmanaged<IntPtr, ArrayInfo, IntPtr> s_Locale_TranslateDynamic = null;
        private static delegate* unmanaged<ulong, ComponentType, void*> s_Entity_CreateComponent = null;
        private static delegate* unmanaged<ulong, ComponentType, void*> s_Entity_GetComponent = null;
        private static delegate* unmanaged<ulong, ComponentType, int> s_Entity_HasComponent = null;
        private static delegate* unmanaged<ulong, ComponentType, void> s_Entity_RemoveComponent = null;
        private static delegate* unmanaged<IntPtr> s_Scene_GetActive = null;
        private static delegate* unmanaged<IntPtr, void> s_Scene_SetActive = null;

        enum ReflectionType : UInt32
        {
            None = 0x00,
            Void = 0x01,
            Boolean = 0x02,
            Char = 0x03,
            SByte = 0x04,
            Byte = 0x05,
            Int16 = 0x06,
            UInt16 = 0x07,
            Int32 = 0x08,
            UInt32 = 0x09,
            Int64 = 0x0a,
            UInt64 = 0x0b,
            Single = 0x0c,
            Double = 0x0d,
            String = 0x0e,
            Ptr = 0x0f,
            Dictionary = 0x10,
            Array = 0x11,
            List = 0x12,
            Object = 0x13,


            Vector2 = 0x20,
            Vector3 = 0x21,
            Vector4 = 0x22,
            Color = 0x23,
            Entity = 0x24,

            AssetHandle = 0x25,
            Asset = 0x26,
            Texture2D = 0x27,
            Font = 0x28,
            Prefab = 0x29,
            Scene = 0x2a,
        }
        /// <summary>
        /// Struct for passing reflection type info to C++ side
        /// it holds information about the type of the argument 
        /// and the pointer to the argument or the data itself if it is a value type, 
        /// that is less than 8 bytes
        /// 
        /// IMPORTANT: If this type is changed, the corresponding C++ type in
        /// ScriptGlue.cpp MUST be changed as well
        ///
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        struct ReflectionTypeInfo
        {
            public ReflectionType Type;
            public IntPtr Ptr;
        }
        /// <summary>
        /// Struct for passing Array info to C++ side
        /// it holds pointer to data and the length of the array
        /// 
        /// IMPORTANT: If this type is changed, the corresponding C++ type in
        /// ScriptGlue.h MUST be changed as well
        ///
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        struct ArrayInfo
        {
            public IntPtr Ptr;
            public ulong Length;
        }
        /// <summary>
        /// Assigns the function pointer to the corresponding function
        /// based on the name of the function
        /// 
        /// IMPORTANT: If a new function is added, it MUST be added here
        /// and to the ScriptGlue.cpp
        /// </summary>
        /// <param name="name"></param>
        /// <param name="functionPtr"></param>
        /// <exception cref="NotImplementedException"></exception>
        private static void AssignFunction(IntPtr name, IntPtr functionPtr)
        {
            GCHandle gCHandle = GCHandle.FromIntPtr(name);
            string? functionName = gCHandle.Target as string;
            Debug.WriteLine("Registering native function " + functionName ?? "null");
            if (functionName == "Log_Warn")
            {
                s_Log_Warn = (delegate* unmanaged<IntPtr, void>)functionPtr;
            }
            else if (functionName == "Log_Error")
            {
                s_Log_Error = (delegate* unmanaged<IntPtr, void>)functionPtr;
            }
            else if (functionName == "Log_Info")
            {
                s_Log_Info = (delegate* unmanaged<IntPtr, void>)functionPtr;
            }
            else if (functionName == "Log_Trace")
            {
                s_Log_Trace = (delegate* unmanaged<IntPtr, void>)functionPtr;
            }
            else if (functionName == "Entity_FindEntityByName")
            {
                s_Entity_FindEntityByName = (delegate* unmanaged<IntPtr, ulong>)functionPtr;
            }
            else if (functionName == "Entity_GetParent")
            {
                s_Entity_GetParent = (delegate* unmanaged<ulong, ulong>)functionPtr;
            }
            else if (functionName == "Entity_SetParent")
            {
                s_Entity_SetParent = (delegate* unmanaged<ulong, ulong, void>)functionPtr;
            }
            else if (functionName == "Entity_GetNextChild")
            {
                s_Entity_GetNextChild = (delegate* unmanaged<ulong, ulong, ulong>)functionPtr;
            }
            else if (functionName == "Entity_HasChild")
            {
                s_Entity_HasChild = (delegate* unmanaged<ulong, ulong, int>)functionPtr;
            }
            else if (functionName == "Entity_AddChild")
            {
                s_Entity_AddChild = (delegate* unmanaged<ulong, ulong, void>)functionPtr;
            }
            else if (functionName == "Entity_RemoveChild")
            {
                s_Entity_RemoveChild = (delegate* unmanaged<ulong, ulong, void>)functionPtr;
            }
            else if (functionName == "Entity_GetName")
            {
                s_Entity_GetName = (delegate* unmanaged<ulong, IntPtr>)functionPtr;
            }
            else if (functionName == "Entity_SetName")
            {
                s_Entity_SetName = (delegate* unmanaged<ulong, IntPtr, void>)functionPtr;
            }
            else if (functionName == "TextRendererComponent_GetText")
            {
                s_TextRendererComponent_GetText = (delegate* unmanaged<ulong, IntPtr>)functionPtr;
            }
            else if (functionName == "TextRendererComponent_SetText")
            {
                s_TextRendererComponent_SetText = (delegate* unmanaged<ulong, IntPtr, void>)functionPtr;
            }
            else if (functionName == "Entity_Destroy")
            {
                s_Entity_Destroy = (delegate* unmanaged<ulong, void>)functionPtr;
            }
            else if (functionName == "Entity_Duplicate")
            {
                s_Entity_Duplicate = (delegate* unmanaged<ulong, ulong>)functionPtr;
            }
            else if (functionName == "Entity_InstantiatePrefab")
            {
                s_Entity_InstantiatePrefab = (delegate* unmanaged<void*, ulong, ulong>)functionPtr;
            }
            else if (functionName == "Input_IsKeyDown")
            {
                s_Input_IsKeyDown = (delegate* unmanaged<int, int>)functionPtr;
            }
            else if (functionName == "Input_IsMouseButtonDown")
            {
                s_Input_IsMouseButtonDown = (delegate* unmanaged<int, int>)functionPtr;
            }
            else if (functionName == "Input_GetMousePosition")
            {
                s_Input_GetMousePosition = (delegate* unmanaged<void*, void>)functionPtr;
            }
            else if (functionName == "Input_GetMousePositionInWorldSpace")
            {
                s_Input_GetMousePositionInWorldSpace = (delegate* unmanaged<ulong, void*, void>)functionPtr;
            }
            else if (functionName == "Asset_Load")
            {
                s_Asset_Load = (delegate* unmanaged<void*, void>)functionPtr;
            }
            else if (functionName == "Asset_Unload")
            {
                s_Asset_Unload = (delegate* unmanaged<void*, void>)functionPtr;
            }
            else if (functionName == "Asset_IsValid")
            {
                s_Asset_IsValid = (delegate* unmanaged<void*, int>)functionPtr;
            }
            else if (functionName == "Asset_IsLoaded")
            {
                s_Asset_IsLoaded = (delegate* unmanaged<void*, int>)functionPtr;
            }
            else if (functionName == "Physics2D_CastRay")
            {
                s_Physics2D_CastRay = (delegate* unmanaged<void*, void*, ulong>)functionPtr;
            }
            else if (functionName == "Locale_GetLocale")
            {
                s_Locale_GetLocale = (delegate* unmanaged<IntPtr>)functionPtr;
            }
            else if (functionName == "Locale_SetLocale")
            {
                s_Locale_SetLocale = (delegate* unmanaged<IntPtr, void>)functionPtr;
            }
            else if (functionName == "Locale_TranslateStatic")
            {
                s_Locale_TranslateStatic = (delegate* unmanaged<IntPtr, IntPtr>)functionPtr;
            }
            else if (functionName == "Locale_TranslateDynamic")
            {
                s_Locale_TranslateDynamic = (delegate* unmanaged<IntPtr, ArrayInfo, IntPtr>)functionPtr;
            }
            else if (functionName == "Entity_CreateComponent")
            {
                s_Entity_CreateComponent = (delegate* unmanaged<ulong, ComponentType, void*>)functionPtr;
            }
            else if (functionName == "Entity_GetComponent")
            {
                s_Entity_GetComponent = (delegate* unmanaged<ulong, ComponentType, void*>)functionPtr;
            }
            else if (functionName == "Entity_HasComponent")
            {
                s_Entity_HasComponent = (delegate* unmanaged<ulong, ComponentType, int>)functionPtr;
            }
            else if (functionName == "Entity_RemoveComponent")
            {
                s_Entity_RemoveComponent = (delegate* unmanaged<ulong, ComponentType, void>)functionPtr;
            }
            else if (functionName == "Scene_GetActive")
            {
                s_Scene_GetActive = (delegate* unmanaged<IntPtr>)functionPtr;
            }
            else if (functionName == "Scene_SetActive")
            {
                s_Scene_SetActive = (delegate* unmanaged<IntPtr, void>)functionPtr;
            }
            else
                throw new NotImplementedException($"Function {functionName} is not implemented in C# on Engine side");
            Debug.WriteLine($"Native function {functionName} registered");
        }

        internal static void Log_Warn(string message)
        {
            s_Log_Warn(Marshal.StringToHGlobalUni(message));
        }

        internal static void Log_Error(string message)
        {
            s_Log_Error(Marshal.StringToHGlobalUni(message));
        }

        internal static void Log_Info(string message)
        {
            s_Log_Info(Marshal.StringToHGlobalUni(message));
        }

        internal static void Log_Trace(string message)
        {
            s_Log_Trace(Marshal.StringToHGlobalUni(message));
        }
        /// <summary>
        /// Enum for component types
        /// IMPORTANT: If this enum is changed, the corresponding C++ enum in
        /// ScriptGlue.h and the corresponding switch case in ScriptGlue.cpp
        /// MUST be changed as well
        /// </summary>
        enum ComponentType: UInt32
        {
            Transform = 0x00,
            SpriteRenderer = 0x01,
            TextRenderer = 0x02,
            BoxCollider2D = 0x03,
            Rigidbody2D = 0x04,
            CircleRenderer = 0x05,
        }
        private static readonly Dictionary<Type, ComponentType> s_ComponentTypeMap = new Dictionary<Type, ComponentType>
        {
            { typeof(TransformComponent), ComponentType.Transform },
            { typeof(SpriteRendererComponent), ComponentType.SpriteRenderer },
            { typeof(TextRendererComponent), ComponentType.TextRenderer },
            { typeof(BoxCollider2DComponent), ComponentType.BoxCollider2D },
            { typeof(RigidBody2DComponent), ComponentType.Rigidbody2D },
            { typeof(CircleRendererComponent), ComponentType.CircleRenderer },
        };
        internal static void* Entity_CreateComponent(ulong id, Type type)
        {
            if (!s_ComponentTypeMap.TryGetValue(type, out ComponentType componentType))
                throw new NotImplementedException($"Component {type} is not implemented in C# on Engine side");
            return s_Entity_CreateComponent(id, componentType);
        }

        internal static void* Entity_GetComponent(ulong id, Type type)
        {
            if (!s_ComponentTypeMap.TryGetValue(type, out ComponentType componentType))
                throw new NotImplementedException($"Component {type} is not implemented in C# on Engine side");
            return s_Entity_GetComponent(id, componentType);
        }

        internal static bool Entity_HasComponent(ulong id, Type type)
        {
            if (!s_ComponentTypeMap.TryGetValue(type, out ComponentType componentType))
                throw new NotImplementedException($"Component {type} is not implemented in C# on Engine side");
            return s_Entity_HasComponent(id, componentType) != 0;
        }

        internal static void Entity_RemoveComponent(ulong id, Type type)
        {
            if (!s_ComponentTypeMap.TryGetValue(type, out ComponentType componentType))
                throw new NotImplementedException($"Component {type} is not implemented in C# on Engine side");
            s_Entity_RemoveComponent(id, componentType);
        }

        internal static ulong Entity_FindEntityByName(string name)
        {
            return s_Entity_FindEntityByName(Marshal.StringToHGlobalUni(name));
        }

        internal static ulong Entity_GetParent(ulong id)
        {
            return s_Entity_GetParent(id);
        }

        internal static void Entity_SetParent(ulong child, ulong parent)
        {
            s_Entity_SetParent(child, parent);
        }

        internal static ulong Entity_GetNextChild(ulong parent, ulong prevChild)
        {
            return s_Entity_GetNextChild(parent, prevChild);
        }

        internal static bool Entity_HasChild(ulong parent, ulong child)
        {
            return s_Entity_HasChild(parent, child) != 0;
        }

        internal static void Entity_AddChild(ulong parent, ulong child)
        {
            s_Entity_AddChild(parent, child);
        }

        internal static void Entity_RemoveChild(ulong parent, ulong child)
        {
            s_Entity_RemoveChild(parent, child);
        }

        internal static string Entity_GetName(ulong id)
        {
            GCHandle handle = GCHandle.FromIntPtr(s_Entity_GetName(id));
            string name = (string)handle.Target;
            handle.Free();
            return name;
        }

        internal static void Entity_SetName(ulong id, string name)
        {
            s_Entity_SetName(id, Marshal.StringToHGlobalUni(name));
        }

        internal static string TextRendererComponent_GetText(ulong id)
        {
            GCHandle handle = GCHandle.FromIntPtr(s_TextRendererComponent_GetText(id));
            string text = (string)handle.Target;
            handle.Free();
            return text;
        }

        internal static void TextRendererComponent_SetText(ulong id, string text)
        {
            s_TextRendererComponent_SetText(id, Marshal.StringToHGlobalUni(text));
        }

        internal static void Entity_Destroy(ulong id)
        {
            s_Entity_Destroy(id);
        }

        internal static ulong Entity_Duplicate(ulong id)
        {
            return s_Entity_Duplicate(id);
        }

        internal static ulong Entity_InstantiatePrefab(ref AssetHandle handle, ulong parentId)
        {
            return s_Entity_InstantiatePrefab(Unsafe.AsPointer(ref handle), parentId);
        }

        internal static bool Input_IsKeyDown(Key key)
        {
            return s_Input_IsKeyDown((int)key) == 1;
        }

        internal static bool Input_IsMouseButtonDown(MouseButton button)
        {
            return s_Input_IsMouseButtonDown((int)button) == 1;
        }

        internal static void Input_GetMousePosition(ref Vector2 mouseCoords)
        {
            s_Input_GetMousePosition(Unsafe.AsPointer(ref mouseCoords));
        }

        internal static void Input_GetMousePositionInWorldSpace(ulong id, ref Vector2 mouseCoords)
        {
            s_Input_GetMousePositionInWorldSpace(id, Unsafe.AsPointer(ref mouseCoords));
        }

        internal static void Asset_Load(ref AssetHandle handle)
        {
            s_Asset_Load(Unsafe.AsPointer(ref handle));
        }

        internal static void Asset_Unload(ref AssetHandle handle)
        {
            s_Asset_Unload(Unsafe.AsPointer(ref handle));
        }
        internal static bool Asset_IsValid(ref AssetHandle handle)
        {
            return s_Asset_IsValid(Unsafe.AsPointer(ref handle)) == 1;
        }

        internal static bool Asset_IsLoaded(ref AssetHandle handle)
        {
            return s_Asset_IsLoaded(Unsafe.AsPointer(ref handle)) == 1;
        }

        internal static ulong Physics2D_CastRay(ref Vector2 start, ref Vector2 end)
        {
            return s_Physics2D_CastRay(Unsafe.AsPointer(ref start), Unsafe.AsPointer(ref end));
        }

        internal static string Locale_GetLocale()
        {
            return Marshal.PtrToStringUTF8(s_Locale_GetLocale());
        }

        internal static void Locale_SetLocale(string locale)
        {
            s_Locale_SetLocale(Marshal.StringToHGlobalUni(locale));
        }

        internal static string Locale_TranslateStatic(string key)
        {
            IntPtr result = s_Locale_TranslateStatic(Marshal.StringToHGlobalUni(key));
            GCHandle gCHandle = GCHandle.FromIntPtr(result);
            string resultString = (string)gCHandle.Target;
            gCHandle.Free();
            return resultString;
        }
        private const int MaxReflectionTypeInfos = 1000;
        private static ReflectionTypeInfo[] s_ReflectionTypeInfos = new ReflectionTypeInfo[MaxReflectionTypeInfos];
        private static GCHandle s_ReflectionTypeInfosHandle = GCHandle.Alloc(s_ReflectionTypeInfos, GCHandleType.Pinned);
        private static object s_ReflectionTypeInfosLock = new object();
        internal static string Locale_TranslateDynamic(string key, object[] args)
        {
            Log.AssertAndThrow(args.Length <= MaxReflectionTypeInfos, "Too many arguments for dynamic translation");
            IntPtr keyPtr = Marshal.StringToHGlobalUni(key);
            ArrayInfo arrayInfo = new ArrayInfo{ Ptr = s_ReflectionTypeInfosHandle.AddrOfPinnedObject(), Length = (ulong)args.Length };
            IntPtr resultPtr = IntPtr.Zero;
            lock(s_ReflectionTypeInfosLock)
            {
                for (int i = 0; i < args.Length; i++)
                {
                    ReflectionTypeInfo info = new ReflectionTypeInfo{Type = ReflectionType.None, Ptr = IntPtr.Zero};
                    if (args[i] == null)
                    {
                        info.Type = ReflectionType.None;
                        info.Ptr = IntPtr.Zero;
                    }
                    else if (args[i] is bool)
                    {
                        info.Ptr = ((bool)args[i]) ? (IntPtr)1 : IntPtr.Zero;
                        info.Type = ReflectionType.Boolean;
                    }
                    else if (args[i] is sbyte)
                    {
                        *(sbyte*)&info.Ptr = (sbyte)args[i];
                        info.Type = ReflectionType.SByte;
                    }
                    else if (args[i] is byte)
                    {
                        *(byte*)&info.Ptr = (byte)args[i];
                        info.Type = ReflectionType.Byte;
                    }
                    else if (args[i] is short)
                    {
                        info.Type = ReflectionType.Int16;
                        *(short*)&info.Ptr = (short)args[i];
                    }
                    else if (args[i] is ushort)
                    {
                        info.Type = ReflectionType.UInt16;
                        *(ushort*)&info.Ptr = (ushort)args[i];
                    }
                    else if (args[i] is int)
                    {
                        info.Type = ReflectionType.Int32;
                        *(int*)&info.Ptr = (int)args[i];
                    }
                    else if (args[i] is uint)
                    {
                        info.Type = ReflectionType.UInt32;
                        *(uint*)(&info.Ptr) = (uint)args[i];
                    }
                    else if (args[i] is long)
                    {
                        info.Type = ReflectionType.Int64;
                        *(long*)(&info.Ptr) = (long)args[i];
                    }
                    else if (args[i] is ulong)
                    {
                        info.Type = ReflectionType.UInt64;
                        *(ulong*)(&info.Ptr) = (ulong)args[i];
                    }
                    else if (args[i] is float)
                    {
                        info.Type = ReflectionType.Single;
                        *(float*)(&info.Ptr) = (float)args[i];
                    }
                    else if (args[i] is double)
                    {
                        info.Type = ReflectionType.Double;
                        *(double*)&info.Ptr = (double)args[i];
                    }
                    else if (args[i] is string)
                    {
                        info.Type = ReflectionType.String;
                        info.Ptr = Marshal.StringToHGlobalUni((string)args[i]);
                    }
                    else
                    {
                        //Log.Debug("Type {0} is not implemented in C# on Engine side", args[i].GetType());
                        throw new NotImplementedException($"Type {args[i].GetType()} is not implemented in C# on Engine side");
                    }
                    s_ReflectionTypeInfos[i] = info;
                }
                //Log.Debug("Calling dynamic translation with {0} arguments", args.Length);
                resultPtr = s_Locale_TranslateDynamic(keyPtr, arrayInfo);
            }
            GCHandle gCHandle = GCHandle.FromIntPtr(resultPtr);
            string resultString = (string)gCHandle.Target;
            gCHandle.Free();
            return resultString;
        }

        internal unsafe static AssetHandle Scene_GetActive()
        {
            return *(AssetHandle*)s_Scene_GetActive();
        }

        internal static void Scene_SetActive(ref AssetHandle scene)
        {
            s_Scene_SetActive((nint)Unsafe.AsPointer(ref scene));
        }

    }
}