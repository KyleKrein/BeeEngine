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
        internal static extern bool Input_IsKeyDown(Key key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Input_IsMouseButtonDown(MouseButton button);
    }
}