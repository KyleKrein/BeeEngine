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

                if (!ValidateLocale(value))
                    throw new ArgumentException("Invalid locale format. Locale must be in the format 'language_COUNTRY' or 'language' and must consist of 2 characters for language and 2 characters for country. Example: 'en_US' or 'en'");

                ClearCache();
                s_CachedLocale = value;
                InternalCalls.Locale_SetLocale(value);
            }
        }

        internal static void ClearCache()
        {
            s_CachedLocale = null;
            s_CachedStaticKeys.Clear();
            s_CachedDynamicKeys.Clear();
        }

        private static bool ValidateLocale(string value)
        {
            if (value.Length < 2 || value.Length > 5)
                return false;
            if (value.Length == 2)
                return true;
            if (value.Length == 5)
            {
                return value[2] == '_';
            }

            return false;
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
                if (args[i] is null)
                    throw new ArgumentNullException($"Argument {i} is null");
                if (i % 2 != 0)
                {
                    if (args[i] is string or int or float or double or bool or byte or sbyte or short or ushort or uint or long or ulong)
                        continue;
                    throw new ArgumentException(
                        $"Each value argument in key-value pair must be a string or int or float or double or bool or byte or sbyte or short or ushort or uint or long or ulong. Argument {i} is not one of these types");
                }
                if (args[i] is not string)
                {
                    throw new ArgumentException(
                        $"Each key argument in key-value pair must be a string. Argument {i} is not a string");
                }
            }
        }

        //TODO: Make a cache invalidation method, that can be called when a locale domain is changed (for example from C++ side)
    }
}