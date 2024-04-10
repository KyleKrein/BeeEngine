//
// Created by alexl on 18.10.2023.
//

#pragma once

#include <utility>
#include <variant>

#include "Core/String.h"
#include "Core/CodeSafety/Expects.h"
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
#include "Core/Hash.h"
namespace BeeEngine
{
    class ScriptGlue;
}
namespace BeeEngine::Locale
{
    class Localization
    {
        public:
        const static Localization Default;
        Localization(): Localization("en_Us") {};
        Localization(UTF8String&& locale)
        : m_Locale(std::move(locale)), m_Language(m_Locale.substr(0, 2))
        {
            BeeExpects(m_Locale.size() == 2 || m_Locale.size() == 5);
            if(m_Locale.size() != 5) //does not contain both language and country
            {
                m_Locale += '_' + ToUppercase({m_Locale.begin(), m_Locale.begin() + 1}) + *(m_Locale.begin() + 1);
            }
        }
        Localization(const UTF8String& locale)
        : m_Locale(locale), m_Language(m_Locale.substr(0, 2))
        {
            BeeExpects(m_Locale.size() == 2 || m_Locale.size() == 5);
            if(m_Locale.size() != 5) //does not contain both language and country
            {
                m_Locale += '_' + ToUppercase({m_Locale.begin(), m_Locale.begin() + 1}) + *(m_Locale.begin() + 1);
            }
        }
        Localization(Localization&& other) noexcept
        : m_Locale(std::move(other.m_Locale)), m_Language(std::move(other.m_Language))
        {}
        Localization(const Localization& other)
        : m_Locale(other.m_Locale), m_Language(other.m_Language)
        {}
        Localization& operator=(const Localization& other)
        {
            m_Locale = other.m_Locale;
            m_Language = other.m_Language;
            return *this;
        }
        Localization& operator=(Localization&& other) noexcept
        {
            m_Locale = std::move(other.m_Locale);
            m_Language = std::move(other.m_Language);
            return *this;
        }
        std::string_view GetLanguage() const
        {
            return {m_Locale.begin(), m_Locale.begin() + 2};
        }
        const String& GetLanguageString() const
        {
            return m_Language;
        }
        std::string_view GetCountry() const
        {
            return {m_Locale.begin() + 3, m_Locale.end()};
        }
        const auto& GetLocale() const
        {
            return m_Locale;
        }
        constexpr auto operator <=>(const Localization& other) const noexcept
        {
            return m_Locale <=> other.m_Locale;
        }
        constexpr auto operator <=>(const UTF8String& other) const noexcept
        {
            return m_Locale <=> other;
        }
        constexpr bool operator ==(const Localization& other) const noexcept
        {
            return m_Locale == other.m_Locale;
        }
        uint64_t Hash(uint64_t seed = 0) const
        {
            return std::hash<std::string>{}(m_Locale);
        }
        String ToString() const
        {
            return m_Locale;
        }
        private:
        UTF8String m_Locale;
        UTF8String m_Language;
    };
}
namespace std
{
    template<>
    struct hash<BeeEngine::Locale::Localization>
    {
        std::size_t operator()(const BeeEngine::Locale::Localization& key) const
        {
            return key.Hash();
        }
    };
}
namespace BeeEngine::Locale
{
    Localization GetSystemLocale();
    class Domain
    {
        friend class LocalizationGenerator;
        friend class ImGuiLocalizationPanel;
        friend BeeEngine::ScriptGlue;

        using Locale = Localization;
        using KeyMap = std::unordered_map<UTF8String, UTF8String>;
        using LanguageMap = std::unordered_map<UTF8String, KeyMap>;
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
            m_Languages[locale.GetLanguageString()] = KeyMap();
        }
        void AddLocalizationSource(const Locale& locale, const Path& path)
        {
            m_LocalizationSources[locale].push_back(path);
        }
        void AddLocaleKey(const Locale& locale, const UTF8String& key, const UTF8String& value)
        {
            m_Languages[locale.GetLanguageString()][key] = value;
        }
        void SetLocale(const Locale& locale)
        {
            m_Locale = locale;
            if (!m_Languages.contains(locale.GetLanguageString()))
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

            auto localeData = m_Languages.find(m_Locale.GetLanguageString());
            if (localeData == m_Languages.end())
            {
                return key;
            }

            auto keyData = localeData->second.find(key);
            if (keyData == localeData->second.end())
            {
                return key;
            }

            UTF8String& pattern = keyData->second;
            UErrorCode status = U_ZERO_ERROR;
            icu::MessageFormat msgFmt(icu::UnicodeString::fromUTF8(pattern), icu::Locale(m_Locale.GetLocale().c_str()), status);

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

            auto localeData = m_Languages.find(m_Locale.GetLanguageString());
            if (localeData == m_Languages.end())
            {
                return key;
            }

            auto keyData = localeData->second.find(key);
            if (keyData == localeData->second.end())
            {
                return key;
            }

            UTF8String& pattern = keyData->second;
            UErrorCode status = U_ZERO_ERROR;
            icu::MessageFormat msgFmt(icu::UnicodeString::fromUTF8(pattern), icu::Locale(m_Locale.GetLocale().c_str()), status);

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