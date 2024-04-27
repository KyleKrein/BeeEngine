//
// Created by Александр Лебедев on 19.10.2023.
//

#pragma once
#include "Core/String.h"
#include "Core/Path.h"

namespace BeeEngine::Locale
{
    class Domain;
    class Localization;
    class LocalizationGenerator final
    {
    public:
        static String GenerateLocalization(const Domain& domain, const Localization& locale);
        static void CreateLocalizationFile(const Path& path, const String& content);
        static void CreateLocalizationFile(const Domain& domain, const Localization& locale, const Path& path);
        static std::vector<Path> GetLocalizationFiles(const Path& path);
        static void ProcessLocalizationFile(Domain& domain, const Path& path);
        static void ProcessLocalizationFiles(Domain& domain, const std::vector<Path>& paths);
    };
}