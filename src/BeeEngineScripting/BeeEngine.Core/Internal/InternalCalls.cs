using BeeEngine.Math;
using System;
using System.Collections.Generic;
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

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void* Entity_CreateComponent(ulong id, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void* Entity_GetComponent(ulong id, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Entity_HasComponent(ulong id, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_RemoveComponent(ulong id, Type type);

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
            return Marshal.PtrToStringUTF8(s_Entity_GetName(id));
        }

        internal static void Entity_SetName(ulong id, string name)
        {
            s_Entity_SetName(id, Marshal.StringToHGlobalUni(name));
        }

        internal static string TextRendererComponent_GetText(ulong id)
        {
            return Marshal.PtrToStringUTF8(s_TextRendererComponent_GetText(id));
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
            return Marshal.PtrToStringUTF8(s_Locale_TranslateStatic(Marshal.StringToHGlobalUni(key)));
        }
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string Locale_TranslateDynamic(string key, object[] args);
    }
}