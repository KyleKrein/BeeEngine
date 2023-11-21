//
// Created by alexl on 18.10.2023.
//

#pragma once

#include <utility>
#include <variant>

#include "Core/String.h"
#include "Core/ToString.h"
#include "Core/Logging/Log.h"
#include "Core/Path.h"
#include <unordered_map>

#include <unicode/msgfmt.h>
#include <unicode/unistr.h>
#include <unicode/locid.h>
#include <unicode/ustream.h>
#include <coroutine>
#include "Core/Coroutines/Generator.h"
namespace BeeEngine
{
    class ScriptGlue;
}
namespace BeeEngine::Locale
{
    UTF8String GetSystemLocale();
    class Domain
    {
        friend class LocalizationGenerator;
        friend class ImGuiLocalizationPanel;
        friend BeeEngine::ScriptGlue;

        using Locale = UTF8String;
        using ValueVariationsMap = std::unordered_map<UTF8String, UTF8String>;
        using KeyMap = std::unordered_map<UTF8String, ValueVariationsMap>;
        using LanguageMap = std::unordered_map<Locale, KeyMap>;
    public:
        Domain(const UTF8String& name)
        : m_Name(name)
        {
            SetLocale(GetSystemLocale());
            BeeCoreTrace("Domain {} was created with default locale {}", name, m_Locale);
        }
        Domain(UTF8String&& name)
        : m_Name(std::move(name))
        {
            SetLocale(GetSystemLocale());
            BeeCoreTrace("Domain {} was created with default locale {}", m_Name, m_Locale);
        }
        Generator<const String&> GetLocales()
        {
            for (auto& [locale, _] : m_Languages)
            {
                co_yield locale;
            }
        }
        void AddLocale(const Locale& locale)
        {
            m_Languages[locale] = KeyMap();
        }
        void AddLocale(Locale&& locale)
        {
            m_Languages[std::move(locale)] = KeyMap();
        }
        void AddLocalizationSource(const Locale& locale, const Path& path)
        {
            m_LocalizationSources[locale].push_back(path);
        }
        void AddLocaleKey(const Locale& locale, const UTF8String& key, const UTF8String& value, const UTF8String& variation = "default")
        {
            m_Languages[locale][key][variation] = value;
        }
        void SetLocale(const Locale& locale)
        {
            m_Locale = locale;
            if (!m_Languages.contains(locale))
            {
                AddLocale(locale);
            }
            BeeCoreTrace("For Domain {} locale {} was set", m_Name, locale);
        }
        const Locale& GetLocale() const
        {
            return m_Locale;
        }
        void Build();
        UTF8String Translate(const char* key);


        template<typename... Args>
        UTF8String Translate(const char *key, Args&&... args)
        {
            static_assert(sizeof...(args) % 2 == 0, "Translate() requires an even number of arguments for key-value pairs.");
            AreKeysStrings<Args...>(std::index_sequence_for<Args...>{});

            auto localeData = m_Languages.find(m_Locale);
            if (localeData == m_Languages.end())
            {
                return key;
            }

            auto keyData = localeData->second.find(key);
            if (keyData == localeData->second.end() || !keyData->second.contains("default"))
            {
                return key;
            }

            UTF8String pattern = keyData->second["default"];
            UErrorCode status = U_ZERO_ERROR;
            icu::MessageFormat msgFmt(icu::UnicodeString::fromUTF8(pattern), icu::Locale(m_Locale.c_str()), status);

            if(!U_SUCCESS(status))
            {
                BeeCoreError("ICU error: {}", u_errorName(status));
                return key;
            }

            // Разбиваем пары ключ-значение на два массива
            std::array<icu::UnicodeString, sizeof...(args) / 2> keys;
            std::array<icu::Formattable, sizeof...(args) / 2> values;

            auto fill_arrays = [&](auto&&... kv)
            {
                size_t index = 0;
                ([&index, &keys, &values](auto&& arg)
                {
                    if(index % 2 == 0)
                    {
                        keys[index++/2] = icu::UnicodeString::fromUTF8(ToString(arg));
                    }
                    else
                    {
                        values[index++/2] = icu::Formattable(arg);
                    }
                }(kv), ...);
                //(((index % 2 == 0 ? keys : values)[index++ / 2] = icu::Formattable(icu::UnicodeString::fromUTF8(ToString(kv)))), ...);
            };

            fill_arrays(std::forward<Args>(args)...);

            icu::UnicodeString icuResult;
            msgFmt.format(keys.data(), values.data(), keys.size(), icuResult, status);

            if(!U_SUCCESS(status))
            {
                BeeCoreError("ICU error: {}", u_errorName(status));
                return key;
            }

            std::string icuOutput;
            icuResult.toUTF8String(icuOutput);

            return icuOutput;
        }
    private:
        template<typename... Args>
        UTF8String TranslateRuntime(const char *key, std::vector<std::variant<Args...>>& args)
        {
            //static_assert(sizeof...(args) % 2 == 0, "Translate() requires an even number of arguments for key-value pairs.");
            //AreKeysStrings<Args...>(std::index_sequence_for<Args...>{});

            auto localeData = m_Languages.find(m_Locale);
            if (localeData == m_Languages.end())
            {
                return key;
            }

            auto keyData = localeData->second.find(key);
            if (keyData == localeData->second.end() || !keyData->second.contains("default"))
            {
                return key;
            }

            UTF8String pattern = keyData->second["default"];
            UErrorCode status = U_ZERO_ERROR;
            icu::MessageFormat msgFmt(icu::UnicodeString::fromUTF8(pattern), icu::Locale(m_Locale.c_str()), status);

            if(!U_SUCCESS(status))
            {
                BeeCoreError("ICU error: {}", u_errorName(status));
                return key;
            }

            // Разбиваем пары ключ-значение на два массива
            std::vector<icu::UnicodeString> keys;
            keys.reserve(args.size() / 2);
            std::vector<icu::Formattable> values;
            values.reserve(args.size() / 2);
            for (auto& v : args)
            {
                std::visit([&keys, &values](auto&& arg){
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, UTF8String>)
                    {
                        keys.push_back(icu::UnicodeString::fromUTF8(arg));
                    }
                    else if constexpr (std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t>)
                    {
                        values.push_back(icu::Formattable((int64_t)arg));
                    }
                    else
                    {
                        values.push_back(icu::Formattable(arg));
                    }
                }, v);
            }

            icu::UnicodeString icuResult;
            msgFmt.format(keys.data(), values.data(), keys.size(), icuResult, status);

            if(!U_SUCCESS(status))
            {
                BeeCoreError("ICU error: {}", u_errorName(status));
                return key;
            }

            std::string icuOutput;
            icuResult.toUTF8String(icuOutput);

            return icuOutput;
        }


        template <typename... Args, size_t... Indices>
        inline void AreKeysStrings(std::index_sequence<Indices...>)
        {
            static_assert(((Indices % 2 != 0 || std::is_convertible_v<std::tuple_element_t<Indices, std::tuple<Args...>>, String>) && ...),
                          "Translate() requires key-value pairs, where key is convertible to String.");
        }
        void RecalculateHash();
        void RebuildFontAtlases();
        uint64_t m_Hash = 0;
        UTF8String m_Name;
        std::unordered_map<Locale, std::vector<Path>> m_LocalizationSources;
        Locale m_Locale;
        LanguageMap m_Languages;
    };
}