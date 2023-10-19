//
// Created by Александр Лебедев on 19.10.2023.
//

#pragma once
#include "Core/String.h"
#include "Core/Path.h"

namespace BeeEngine::Locale
{
    class Domain;
    class LocalizationGenerator final
    {
    public:
        static String GenerateLocalization(const Domain& domain, const String& locale);
        static void CreateLocalizationFile(const Path& path, const String& content);
        static void CreateLocalizationFile(const Domain& domain, const String& locale, const Path& path);
    };
}