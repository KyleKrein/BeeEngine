using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace BeeEngine.Internal
{
    public static class Unsafe
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static unsafe ref T AsRef<T>(void* source)
        {
            return ref *(T*)source;
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static unsafe bool IsNullRef<T>(ref T source)
        {
            fixed (void* ptr = &source)
                return ptr == (void*)0;
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static unsafe ref T NullRef<T>()
        {
            return ref *(T*)0;
        }
    }
}