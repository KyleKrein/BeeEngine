using System;
using System.Diagnostics;
using System.Diagnostics.CodeAnalysis;
using System.Runtime.CompilerServices;
using BeeEngine.Internal;

namespace BeeEngine
{
    public static class Log
    {
        private static LogLevel LogLevel { get; set; } = LogLevel.Trace;

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Info(string o)
        {
            if (LogLevel >= LogLevel.Information)
                InternalCalls.Log_Info(o);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Debug(string o)
        {
            if (LogLevel >= LogLevel.Trace)
                InternalCalls.Log_Trace(o);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Warning(string o)
        {
            if (LogLevel >= LogLevel.Warning)
                InternalCalls.Log_Warn(o);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Error(string o)
        {
            if (LogLevel >= LogLevel.Error)
                InternalCalls.Log_Error(o);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Info<T>(string format, T arg)
        {
            if (LogLevel >= LogLevel.Information)
            {
                var o = String.Format(format, arg);
                InternalCalls.Log_Info(o);
            }
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Info<T0, T1>(string format, T0 arg0, T1 arg1)
        {
            if (LogLevel >= LogLevel.Information)
            {
                var o = String.Format(format, arg0, arg1);
                InternalCalls.Log_Info(o);
            }
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Info<T0, T1, T2>(string format, T0 arg0, T1 arg1, T2 arg2)
        {
            if (LogLevel >= LogLevel.Information)
            {
                var o = String.Format(format, arg0, arg1, arg2);
                InternalCalls.Log_Info(o);
            }
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Debug<T>(string format, T arg)
        {
            if (LogLevel >= LogLevel.Trace)
            {
                var o = String.Format(format, arg);
                InternalCalls.Log_Trace(o);
            }
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Debug<T0, T1>(string format, T0 arg0, T1 arg1)
        {
            if (LogLevel >= LogLevel.Trace)
            {
                var o = String.Format(format, arg0, arg1);
                InternalCalls.Log_Trace(o);
            }
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Debug<T0, T1, T2>(string format, T0 arg0, T1 arg1, T2 arg2)
        {
            if (LogLevel >= LogLevel.Trace)
            {
                var o = String.Format(format, arg0, arg1, arg2);
                InternalCalls.Log_Trace(o);
            }
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Warning<T>(string format, T arg)
        {
            if (LogLevel >= LogLevel.Warning)
            {
                var o = String.Format(format, arg);
                InternalCalls.Log_Warn(o);
            }
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Warning<T0, T1>(string format, T0 arg0, T1 arg1)
        {
            if (LogLevel >= LogLevel.Warning)
            {
                var o = String.Format(format, arg0, arg1);
                InternalCalls.Log_Warn(o);
            }
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Warning<T0, T1, T2>(string format, T0 arg0, T1 arg1, T2 arg2)
        {
            if (LogLevel >= LogLevel.Warning)
            {
                var o = String.Format(format, arg0, arg1, arg2);
                InternalCalls.Log_Warn(o);
            }
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Error<T>(string format, T arg)
        {
            if (LogLevel >= LogLevel.Error)
            {
                var o = String.Format(format, arg);
                InternalCalls.Log_Error(o);
            }
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Error<T0, T1>(string format, T0 arg0, T1 arg1)
        {
            if (LogLevel >= LogLevel.Error)
            {
                var o = String.Format(format, arg0, arg1);
                InternalCalls.Log_Error(o);
            }
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Error<T0, T1, T2>(string format, T0 arg0, T1 arg1, T2 arg2)
        {
            if (LogLevel >= LogLevel.Error)
            {
                var o = String.Format(format, arg0, arg1, arg2);
                InternalCalls.Log_Error(o);
            }
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Info(string format, params object[] args)
        {
            if (LogLevel >= LogLevel.Information)
            {
                var o = string.Format(format, args);
                InternalCalls.Log_Info(o);
            }
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Debug(string format, params object[] args)
        {
            if (LogLevel >= LogLevel.Trace)
            {
                var o = string.Format(format, args);
                InternalCalls.Log_Trace(o);
            }
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Warning(string format, params object[] args)
        {
            if (LogLevel >= LogLevel.Warning)
            {
                var o = string.Format(format, args);
                InternalCalls.Log_Warn(o);
            }
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Error(string format, params object[] args)
        {
            if (LogLevel >= LogLevel.Information)
            {
                var o = string.Format(format, args);
                InternalCalls.Log_Error(o);
            }
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Assert(bool isOk, string format, params object[] args)
        {
            if (isOk)
            {
                return;
            }

            Error(format, args);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Assert<T>(bool isOk, string format, T arg)
        {
            if (isOk)
            {
                return;
            }

            Error(format, arg);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Assert<T0, T1>(bool isOk, string format, T0 arg0, T1 arg1)
        {
            if (isOk)
            {
                return;
            }

            Error(format, arg0, arg1);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Assert<T0, T1, T2>(bool isOk, string format, T0 arg0, T1 arg1, T2 arg2)
        {
            if (isOk)
            {
                return;
            }

            Error(format, arg0, arg1, arg2);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Assert(bool isOk, string o)
        {
            if (isOk)
            {
                return;
            }

            Error(o);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void AssertAndThrow([DoesNotReturnIf(false)]bool isOk, string format, params object[] args)
        {
            if (isOk)
            {
                return;
            }

            var final = string.Format(format, args);
            Error(final);
            throw new Exception(final);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void AssertAndThrow<T>([DoesNotReturnIf(false)]bool isOk, string format, T arg)
        {
            if (isOk)
            {
                return;
            }

            var final = String.Format(format, arg);
            Error(final);
            throw new Exception(final);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void AssertAndThrow<T0, T1>([DoesNotReturnIf(false)]bool isOk, string format, T0 arg0, T1 arg1)
        {
            if (isOk)
            {
                return;
            }

            var final = String.Format(format, arg0, arg1);
            Error(final);
            throw new Exception(final);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void AssertAndThrow<T0, T1, T2>([DoesNotReturnIf(false)]bool isOk, string format, T0 arg0, T1 arg1, T2 arg2)
        {
            if (isOk)
            {
                return;
            }

            var final = String.Format(format, arg0, arg1, arg2);
            Error(final);
            throw new Exception(final);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void AssertAndThrow([DoesNotReturnIf(false)]bool isOk, string o)
        {
            if (isOk)
            {
                return;
            }

            Error(o);
            throw new Exception(o);
        }
    }

    /// <summary>
    /// Logs messages to the console.
    /// But the function calls are only compiled in debug mode
    /// and completely removed in release mode.
    /// </summary>
    public static class DebugLog
    {
        private static LogLevel LogLevel { get; set; } = LogLevel.Trace;

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Info(string o)
        {
            if (LogLevel >= LogLevel.Information)
                InternalCalls.Log_Info(o);
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Debug(string o)
        {
            if (LogLevel >= LogLevel.Trace)
                InternalCalls.Log_Trace(o);
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Warning(string o)
        {
            if (LogLevel >= LogLevel.Warning)
                InternalCalls.Log_Warn(o);
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Error(string o)
        {
            if (LogLevel >= LogLevel.Error)
                InternalCalls.Log_Error(o);
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Info<T>(string format, T arg)
        {
            if (LogLevel >= LogLevel.Information)
            {
                var o = String.Format(format, arg);
                InternalCalls.Log_Info(o);
            }
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Info<T0, T1>(string format, T0 arg0, T1 arg1)
        {
            if (LogLevel >= LogLevel.Information)
            {
                var o = String.Format(format, arg0, arg1);
                InternalCalls.Log_Info(o);
            }
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Info<T0, T1, T2>(string format, T0 arg0, T1 arg1, T2 arg2)
        {
            if (LogLevel >= LogLevel.Information)
            {
                var o = String.Format(format, arg0, arg1, arg2);
                InternalCalls.Log_Info(o);
            }
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Debug<T>(string format, T arg)
        {
            if (LogLevel >= LogLevel.Trace)
            {
                var o = String.Format(format, arg);
                InternalCalls.Log_Trace(o);
            }
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Debug<T0, T1>(string format, T0 arg0, T1 arg1)
        {
            if (LogLevel >= LogLevel.Trace)
            {
                var o = String.Format(format, arg0, arg1);
                InternalCalls.Log_Trace(o);
            }
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Debug<T0, T1, T2>(string format, T0 arg0, T1 arg1, T2 arg2)
        {
            if (LogLevel >= LogLevel.Trace)
            {
                var o = String.Format(format, arg0, arg1, arg2);
                InternalCalls.Log_Trace(o);
            }
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Warning<T>(string format, T arg)
        {
            if (LogLevel >= LogLevel.Warning)
            {
                var o = String.Format(format, arg);
                InternalCalls.Log_Warn(o);
            }
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Warning<T0, T1>(string format, T0 arg0, T1 arg1)
        {
            if (LogLevel >= LogLevel.Warning)
            {
                var o = String.Format(format, arg0, arg1);
                InternalCalls.Log_Warn(o);
            }
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Warning<T0, T1, T2>(string format, T0 arg0, T1 arg1, T2 arg2)
        {
            if (LogLevel >= LogLevel.Warning)
            {
                var o = String.Format(format, arg0, arg1, arg2);
                InternalCalls.Log_Warn(o);
            }
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Error<T>(string format, T arg)
        {
            if (LogLevel >= LogLevel.Error)
            {
                var o = String.Format(format, arg);
                InternalCalls.Log_Error(o);
            }
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Error<T0, T1>(string format, T0 arg0, T1 arg1)
        {
            if (LogLevel >= LogLevel.Error)
            {
                var o = String.Format(format, arg0, arg1);
                InternalCalls.Log_Error(o);
            }
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Error<T0, T1, T2>(string format, T0 arg0, T1 arg1, T2 arg2)
        {
            if (LogLevel >= LogLevel.Error)
            {
                var o = String.Format(format, arg0, arg1, arg2);
                InternalCalls.Log_Error(o);
            }
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Info(string format, params object[] args)
        {
            if (LogLevel >= LogLevel.Information)
            {
                var o = string.Format(format, args);
                InternalCalls.Log_Info(o);
            }
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Debug(string format, params object[] args)
        {
            if (LogLevel >= LogLevel.Trace)
            {
                var o = string.Format(format, args);
                InternalCalls.Log_Trace(o);
            }
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Warning(string format, params object[] args)
        {
            if (LogLevel >= LogLevel.Warning)
            {
                var o = string.Format(format, args);
                InternalCalls.Log_Warn(o);
            }
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Error(string format, params object[] args)
        {
            if (LogLevel >= LogLevel.Information)
            {
                var o = string.Format(format, args);
                InternalCalls.Log_Error(o);
            }
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Assert(bool isOk, string format, params object[] args)
        {
            if (isOk)
            {
                return;
            }

            Error(format, args);
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Assert<T>(bool isOk, string format, T arg)
        {
            if (isOk)
            {
                return;
            }

            Error(format, arg);
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Assert<T0, T1>(bool isOk, string format, T0 arg0, T1 arg1)
        {
            if (isOk)
            {
                return;
            }

            Error(format, arg0, arg1);
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Assert<T0, T1, T2>(bool isOk, string format, T0 arg0, T1 arg1, T2 arg2)
        {
            if (isOk)
            {
                return;
            }

            Error(format, arg0, arg1, arg2);
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void Assert(bool isOk, string o)
        {
            if (isOk)
            {
                return;
            }

            Error(o);
            Debugger.Break();
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void AssertAndThrow([DoesNotReturnIf(false)]bool isOk, string format, params object[] args)
        {
            if (isOk)
            {
                return;
            }

            var final = string.Format(format, args);
            Error(final);
            throw new Exception(final);
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void AssertAndThrow<T>([DoesNotReturnIf(false)]bool isOk, string format, T arg)
        {
            if (isOk)
            {
                return;
            }

            var final = String.Format(format, arg);
            Error(final);
            throw new Exception(final);
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void AssertAndThrow<T0, T1>([DoesNotReturnIf(false)]bool isOk, string format, T0 arg0, T1 arg1)
        {
            if (isOk)
            {
                return;
            }

            var final = String.Format(format, arg0, arg1);
            Error(final);
            throw new Exception(final);
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void AssertAndThrow<T0, T1, T2>([DoesNotReturnIf(false)]bool isOk, string format, T0 arg0, T1 arg1, T2 arg2)
        {
            if (isOk)
            {
                return;
            }

            var final = String.Format(format, arg0, arg1, arg2);
            Error(final);
            throw new Exception(final);
        }

        [Conditional("DEBUG")]
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void AssertAndThrow([DoesNotReturnIf(false)]bool isOk, string o)
        {
            if (isOk)
            {
                return;
            }

            Error(o);
            throw new Exception(o);
        }
    }
}