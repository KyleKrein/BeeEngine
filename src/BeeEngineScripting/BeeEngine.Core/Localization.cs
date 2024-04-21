using System;
using System.Collections.Generic;
using BeeEngine.Internal;

namespace BeeEngine
{
    public static class Localization
    {
        private static string? s_CachedLocale = null;
        private static Dictionary<string, string> s_CachedStaticKeys = new Dictionary<string, string>();
        private static Dictionary<int, string> s_CachedDynamicKeys = new Dictionary<int, string>();
        public static string Locale
        {
            get
            {
                if (s_CachedLocale is null)
                {
                    s_CachedLocale = InternalCalls.Locale_GetLocale();
                }

                return s_CachedLocale;
            }
            set
            {
                if (value is null)
                    throw new ArgumentNullException(nameof(value));
                if (s_CachedLocale == value)
                {
                    return;
                }

                s_CachedLocale = value;
                InternalCalls.Locale_SetLocale(value);
                s_CachedStaticKeys.Clear();
                s_CachedDynamicKeys.Clear();
            }
        }

        public static string Translate(string key)
        {
            if (key is null)
                throw new ArgumentNullException(nameof(key));
            if (!s_CachedStaticKeys.TryGetValue(key, out var translate))
            {
                translate = InternalCalls.Locale_TranslateStatic(key);
                s_CachedStaticKeys[key] = translate;
            }
            return translate;
        }

        public static string Translate(string key, params object[] args)
        {
            if (key is null)
                throw new ArgumentNullException(nameof(key));
            ValidateArgs(args);
            int hash = key.GetHashCode();
            foreach (var arg in args)
            {
                hash ^= arg.GetHashCode();
            }
            if (!s_CachedDynamicKeys.TryGetValue(hash, out var translate))
            {
                translate = InternalCalls.Locale_TranslateDynamic(key, args);
                s_CachedDynamicKeys[hash] = translate;
            }

            return translate;
        }

        private static void ValidateArgs(object[] args)
        {
            if (args.Length % 2 != 0)
            {
                throw new ArgumentException("Number of arguments must be in pairs [key - string, value - object]");
            }

            for (int i = 0; i < args.Length; i++)
            {
                if(i % 2 != 0)
                    continue;
                if (args[i].GetType() != typeof(string))
                {
                    throw new ArgumentException(
                        $"Each key argument in key-value pair must be a string. Argument {i} is not a string");
                }
            }
        }
    }
}