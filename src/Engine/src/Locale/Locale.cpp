//
// Created by alexl on 18.10.2023.
//
#include "Locale.h"
#include <ranges>
#include <algorithm>
#include "Core/Hash.h"
#include "FileSystem/File.h"
#include "yaml-cpp/yaml.h"


#if defined(WINDOWS)
#include <windows.h>
#include "Platform/Windows/WindowsString.h"
#include "Core/Hash.h"

#elif defined(MACOS) || defined(IOS)
#include <CoreFoundation/CoreFoundation.h>
#elif defined(LINUX)
  #include <cstdlib>
#endif
namespace BeeEngine::Locale
{
    const Localization Localization::Default = {"en_US"};
    Localization GetSystemLocale() {
#if defined(WINDOWS)
      wchar_t lang[LOCALE_NAME_MAX_LENGTH];
      if (GetUserDefaultLocaleName(lang, LOCALE_NAME_MAX_LENGTH)) {
        return {Internal::WStringToUTF8(lang)};
      }
#elif defined(MACOS) || defined(IOS)
      CFArrayRef preferredLangs = CFLocaleCopyPreferredLanguages();
      if (preferredLangs) {
        CFStringRef langRef = (CFStringRef)CFArrayGetValueAtIndex(preferredLangs, 0);
        if (langRef) {
          char lang[256];
          CFStringGetCString(langRef, lang, sizeof(lang), kCFStringEncodingUTF8);
          return {std::string(lang)};
        }
      }
#elif defined(LINUX)
      const char* lang = std::getenv("LANG");
      if (lang) {
        return {std::string(lang)};
      }
#endif
      return Localization::Default; // Fallback to English
    }

    void Domain::Build()
    {
        uint64_t prevHash = m_Hash;
        RecalculateHash();
        if(prevHash != m_Hash)
        {
            RebuildFontAtlases();
        }
    }

    void Domain::RecalculateHash()
    {
        m_Hash = 0;
        std::vector<UTF8String> locales;
        locales.reserve(m_LocalizationSources.size());
        for(auto& [locale, paths] : m_LocalizationSources)
        {
            locales.push_back(locale.GetLocale());
        }
        std::ranges::sort(locales);
        for(auto& locale : locales)
        {
            m_Hash = Hash(locale, m_Hash);
        }
    }

    void Domain::RebuildFontAtlases()
    {

    }

    UTF8String Domain::Translate(const char *key)
    {
        if(m_Languages.contains(m_Locale.GetLanguageString()) &&
                m_Languages[m_Locale.GetLanguageString()].contains(key))
        {
            return m_Languages[m_Locale.GetLanguageString()][key];
        }
        return key;
    }
}