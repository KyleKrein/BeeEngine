//
// Created by alexl on 18.10.2023.
//

#pragma once

#include <utility>

#include "Core/String.h"
#include "Core/Logging/Log.h"
#include "Core/Path.h"
#include <unordered_map>

namespace BeeEngine::Locale
{
    UTF8String GetSystemLocale();
    class Domain
    {
        friend class LocalizationGenerator;
        friend class ImGuiLocalizationPanel;
        using Locale = UTF8String;
        using ValueVariationsMap = std::unordered_map<UTF8String, UTF8String>;
        using KeyMap = std::unordered_map<UTF8String, ValueVariationsMap>;
        using LanguageMap = std::unordered_map<Locale, KeyMap>;
    public:
        Domain(const UTF8String& name)
        : m_Name(name)
        {
            BeeCoreTrace("Domain {} was created", name);
        }
        Domain(UTF8String&& name)
        : m_Name(std::move(name))
        {
            BeeCoreTrace("Domain {} was created", m_Name);
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
        void Build();
        UTF8String Translate(const char* key);
    private:
        void RecalculateHash();
        void RebuildFontAtlases();
        void LoadKeysFromSources();
        void LoadKeysFromSource(const Locale& locale, const Path& path);
        uint64_t m_Hash = 0;
        UTF8String m_Name;
        std::unordered_map<Locale, std::vector<Path>> m_LocalizationSources;
        Locale m_Locale;
        LanguageMap m_Languages;
    };
}