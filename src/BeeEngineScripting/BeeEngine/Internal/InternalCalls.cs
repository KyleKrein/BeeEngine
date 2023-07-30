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
    }
}