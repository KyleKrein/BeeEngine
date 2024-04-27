//
// Created by alexl on 18.10.2023.
//
//@file: src/Engine/src/Locale/Locale.h Main file for localization system.
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
    /**
     * @brief The Localization class represents a locale used for localization purposes.
     * 
     * This class provides functionality to handle and manipulate locale information, such as language and country.
     * It also supports comparison, hashing, and conversion to string.
     */
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
        /**
         * @brief Gets the language of the locale.
         *
         * @return A std::string_view representing the language of the locale.
         */
        std::string_view GetLanguage() const
        {
            return {m_Locale.begin(), m_Locale.begin() + 2};
        }
        /**
         * Retrieves the language string.
         *
         * @return The language string.
         */
        const String& GetLanguageString() const
        {
            return m_Language;
        }
        /**
         * @brief Retrieves the country associated with the locale.
         *
         * This function returns a std::string_view representing the country associated with the locale.
         *
         * @return A std::string_view representing the country associated with the locale.
         */
        std::string_view GetCountry() const
        {
            return {m_Locale.begin() + 3, m_Locale.end()};
        }
        /**
         * Retrieves the locale.
         *
         * @return A reference to the locale.
         */
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
        /**
         * Calculates the hash value of the object.
         *
         * @param seed The seed value for the hash calculation. Default is 0. Unused.
         * @return The hash value of the object.
         */
        uint64_t Hash(uint64_t seed = 0) const
        {
            return std::hash<std::string>{}(m_Locale);
        }
        /**
         * Converts the object to a string representation.
         *
         * @return The string representation of the object.
         */
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
    /**
     * Retrieves the system locale.
     *
     * This function returns the system locale, which represents the language and cultural preferences
     * set on the user's operating system.
     * If the system locale cannot be determined, the default locale is returned.
     *
     * @return The system locale.
     */
    Localization GetSystemLocale();
    /**
     * @brief The Domain class represents a domain for localization in the BeeEngine.
     *
     * A domain is a container for localization data, including locales, keys, and values.
     * It provides methods to add and retrieve localization data, set the current locale,
     * and translate keys into localized strings.
     */
    class Domain
    {
        friend class LocalizationGenerator;
        friend class ImGuiLocalizationPanel;
        friend BeeEngine::ScriptGlue;

        using Locale = Localization;
        using KeyMap = std::unordered_map<UTF8String, UTF8String>;
        using LanguageMap = std::unordered_map<UTF8String, KeyMap>;
    public:
        /**
         * @brief Constructs a new Domain object with the specified name.
         * Default locale is set to the system locale.
         * @param name The name of the domain.
         */
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
        /**
         * Retrieves the locales registerd in the domain.
         *
         * @return A generator that yields constant references to the available locales.
         */
        Generator<const String&> GetLocales()
        {
            for (auto& [locale, _] : m_Languages)
            {
                co_yield locale;
            }
        }
        /**
         * Adds a new locale to the collection.
         *
         * @param locale The locale to be added.
         */
        void AddLocale(const Locale& locale)
        {
            m_Languages[locale.GetLanguageString()] = KeyMap();
        }
        /**
         * @brief Adds a localization source to the engine.
         *
         * This function adds a localization source to the engine, allowing it to load and use localized resources.
         *
         * @param locale The locale to associate with the localization source.
         * @param path The path to the localization source.
         */
        void AddLocalizationSource(const Locale& locale, const Path& path)
        {
            m_LocalizationSources[locale].push_back(path);
        }
        /**
         * Adds a key-value pair to the specified locale.
         *
         * @param locale The locale to add the key-value pair to.
         * @param key The key of the pair.
         * @param value The value of the pair.
         */
        void AddLocaleKey(const Locale& locale, const UTF8String& key, const UTF8String& value)
        {
            m_Languages[locale.GetLanguageString()][key] = value;
        }
        /**
         * Sets the locale to be used by the domain.
         * If the locale is not registered in the domain, it is added.
         *
         * @param locale The locale to be set.
         */
        void SetLocale(const Locale& locale)
        {
            m_Locale = locale;
            if (!m_Languages.contains(locale.GetLanguageString()))
            {
                AddLocale(locale);
            }
            BeeCoreTrace("For Domain {} locale {} was set", m_Name, locale);
        }
        /**
         * Retrieves the current locale.
         *
         * @return The current locale.
         */
        const Locale& GetLocale() const
        {
            return m_Locale;
        }
        void Build();
        /**
         * Translates the given key into a UTF8String, using current Locale.
         *
         * @param key The key to be translated.
         * @return The translated string.
         */
        UTF8String Translate(const char* key) const;


        template<typename... Args>
        /**
         * Translates the given key into the localized string using the specified arguments.
         *
         * @param key The key to be translated.
         * @param args The arguments to be used for formatting the translated string. The arguments must be key-value pairs.
         * @return The translated string.
         */
        UTF8String Translate(const char *key, Args&&... args) const
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

            const UTF8String& pattern = keyData->second;
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
        UTF8String TranslateRuntime(const char *key, std::vector<std::variant<Args...>>& args) const
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

            const UTF8String& pattern = keyData->second;
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
        inline void AreKeysStrings(std::index_sequence<Indices...>) const
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