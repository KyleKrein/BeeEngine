//
// Created by alexl on 18.10.2023.
//
#include "Locale.h"
#include <ranges>



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
    UTF8String GetSystemLocale() {
#if defined(WINDOWS)
      wchar_t lang[LOCALE_NAME_MAX_LENGTH];
      if (GetUserDefaultLocaleName(lang, LOCALE_NAME_MAX_LENGTH)) {
        return Internal::WStringToUTF8(lang);
      }
#elif defined(MACOS) || defined(IOS)
      CFArrayRef preferredLangs = CFLocaleCopyPreferredLanguages();
      if (preferredLangs) {
        CFStringRef langRef = (CFStringRef)CFArrayGetValueAtIndex(preferredLangs, 0);
        if (langRef) {
          char lang[256];
          CFStringGetCString(langRef, lang, sizeof(lang), kCFStringEncodingUTF8);
          return std::string(lang);
        }
      }
#elif defined(LINUX)
      const char* lang = std::getenv("LANG");
      if (lang) {
        return std::string(lang);
      }
#endif
      return "en_US"; // Fallback to English
    }

    void Domain::Build()
    {
        uint64_t prevHash = m_Hash;
        RecalculateHash();
        if(prevHash != m_Hash)
        {
            RebuildFontAtlases();
        }
        LoadKeysFromSources();
    }

    void Domain::RecalculateHash()
    {
        m_Hash = 0;
        std::vector<UTF8String> locales;
        locales.reserve(m_LocalizationSources.size());
        for(auto& [locale, paths] : m_LocalizationSources)
        {
            locales.push_back(locale);
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

    void Domain::LoadKeysFromSources()
    {

    }

    UTF8String Domain::Translate(const char *key)
    {
        if(m_Languages.contains(m_Locale) &&
                m_Languages[m_Locale].contains(key) &&
                m_Languages[m_Locale][key].contains("default"))
        {
            return m_Languages[m_Locale][key]["default"];
        }
        return key;
    }
}