using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace BeeEngine.Internal
{
    public static class UnsafeMethods
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static unsafe ref T AsRef<T>(void* source) where T: unmanaged
        {
            return ref *(T*)source;
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static unsafe bool IsNullRef<T>(ref T source) where T: unmanaged
        {
            fixed (void* ptr = &source)
                return ptr == (void*)0;
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static unsafe ref T NullRef<T>() where T: unmanaged
        {
            return ref *(T*)0;
        }
    }
}