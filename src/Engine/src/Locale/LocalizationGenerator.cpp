//
// Created by Александр Лебедев on 19.10.2023.
//

#include "LocalizationGenerator.h"
#include "FileSystem/File.h"
#include <yaml-cpp/yaml.h>
#include "Locale.h"
namespace BeeEngine::Locale
{
    String LocalizationGenerator::GenerateLocalization(const Domain &domain, const String &locale)
    {
        // Проверка наличия локали в данных
        if (!domain.m_Languages.contains(locale))
        {
            throw std::runtime_error("Locale not found");
        }

        YAML::Emitter out;
        out << YAML::BeginMap;

        const auto& keyMap = domain.m_Languages.at(locale);
        for (const auto& [key, values] : keyMap)
        {
            // Проверка на наличие вариаций (склонения, множественное число и т.д.)
            if (values.size() == 1)
            {
                out << YAML::Key << key;
                out << YAML::Value << values.at("default");
            }
            else
            {
                out << YAML::Key << key;
                out << YAML::Value << YAML::BeginMap;

                for (const auto& [variation, value] : values)
                {
                    out << YAML::Key << variation << YAML::Value << value;
                }

                out << YAML::EndMap;
            }
        }

        out << YAML::EndMap;
        return out.c_str();
    }

    void LocalizationGenerator::CreateLocalizationFile(const Path &path, const String &content)
    {
        File::WriteFile(path, content);
    }

    void LocalizationGenerator::CreateLocalizationFile(const Domain &domain, const String &locale, const Path &path)
    {
        CreateLocalizationFile(path, GenerateLocalization(domain, locale));
    }
}