using BeeEngine.Math;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace BeeEngine.Internal
{
    internal static class InternalCalls
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Log_Warn(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Log_Error(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Log_Info(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Log_Trace(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_GetTranslation(ulong id, out Vector3 vec);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern unsafe void* Entity_CreateComponent(ulong id, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern unsafe void* Entity_GetComponent(ulong id, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Entity_HasComponent(ulong id, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_RemoveComponent(ulong id, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern unsafe void* Entity_GetTransformComponent(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_SetTranslation(ulong id, ref Vector3 vec);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern ulong Entity_FindEntityByName(string name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern ulong Entity_GetParent(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_SetParent(ulong child, ulong parent);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern ulong Entity_GetNextChild(ulong parent, ulong prevChild);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Entity_HasChild(ulong parent, ulong child);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_AddChild(ulong parent, ulong child);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_RemoveChild(ulong parent, ulong child);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string Entity_GetName(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_SetName(ulong id, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern object Entity_GetScriptInstance(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string TextRendererComponent_GetText(ulong id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void TextRendererComponent_SetText(ulong id, string text);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_Destroy(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern ulong Entity_Duplicate(ulong id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern ulong Entity_InstantiatePrefab(ref AssetHandle handle, ulong parentId);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Input_IsKeyDown(Key key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Input_IsMouseButtonDown(MouseButton button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Input_GetMousePosition(ref Vector2 mouseCoords);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Input_GetMousePositionInWorldSpace(ulong id, ref Vector2 mouseCoords);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Asset_Load(ref AssetHandle handle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Asset_Unload(ref AssetHandle handle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Asset_IsValid(ref AssetHandle handle);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Asset_IsLoaded(ref AssetHandle handle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern ulong Physics2D_CastRay(ref Vector2 start, ref Vector2 end);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string Locale_GetLocale();
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Locale_SetLocale(string locale);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string Locale_TranslateStatic(string key);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string Locale_TranslateDynamic(string key, object[] args);
    }
}