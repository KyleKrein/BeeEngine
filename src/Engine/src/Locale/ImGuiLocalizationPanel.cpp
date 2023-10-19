//
// Created by Александр Лебедев on 19.10.2023.
//

#include "ImGuiLocalizationPanel.h"
#include "LocalizationGenerator.h"
#include "Gui/ImGui/ImGuiExtension.h"
#include "Locale.h"
#include <array>
namespace BeeEngine::Locale
{
    ImGuiLocalizationPanel::ImGuiLocalizationPanel(Domain& domain, const Path& path)
    : m_Domain(&domain), m_SelectedLocale(domain.m_Locale), m_WorkingDirectory(path)
    {
        UpdateLocaleKeys();
    }
    void ImGuiLocalizationPanel::Render()
    {
        if(!m_IsOpened)
            return;
        ImGui::Begin("Localization", &m_IsOpened);
        auto& locales = m_LocaleKeys;
        // Верхний переключатель локализаций
        if (ImGui::BeginCombo("Locales", m_SelectedLocale.c_str()))
        {
            for(auto& [locale, keys] : locales)
            {
                bool isSelected = (m_SelectedLocale == locale);
                if (ImGui::Selectable(locale.c_str(), isSelected))
                {
                    m_SelectedLocale = locale;
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        if(ImGui::Button("+"))
        {
            ImGui::OpenPopup("Add Locale");
            ImGui::Text("Name");
            ImGui::SameLine();
            static std::array<char, 128> newLocale{'\0'};
            ImGui::InputText("##New Locale", newLocale.data(), newLocale.size());
            if(ImGui::Button("Add"))
            {
                m_LocaleKeys.insert({newLocale.data(), {}});
                newLocale.fill('\0');
                ImGui::CloseCurrentPopup();
            }
        }
        if(locales.size() > 1)
        {
            ImGui::SameLine();
            if(ImGui::Button("-"))
            {
                ImGui::OpenPopup("Remove Locale");
                ImGui::Text("Are you sure, that you want to delete locale %s?", m_SelectedLocale.c_str());
                if(ImGui::Button("Yes"))
                {
                    m_LocaleKeys.erase(m_SelectedLocale);
                    m_SelectedLocale = m_LocaleKeys.begin()->first;
                    ImGui::CloseCurrentPopup();
                }
                else
                {
                    ImGui::SameLine();
                    if(ImGui::Button("No"))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                }
            }
        }
        bool removeValue = false;
        bool removeKey = false;
        // Отображение текущих ключей и их локализаций
        ImGui::Text("Localization Keys:");
        for (auto& [locale, keys] : m_LocaleKeys)
        {
            ImGui::Text("Locale: %s", locale.c_str());
            for (auto& [key, values] : keys)
            {
                ImGui::Text("  Key: ");
                ImGui::SameLine();
                ImGui::InputText("##Key", &key);
                ImGui::SameLine();
                if(ImGui::Button("-"))
                {
                    ImGui::OpenPopup("Remove Key");
                    ImGui::Text("Are you sure, that you want to delete key %s?", key.c_str());
                    if(ImGui::Button("Yes"))
                    {
                        m_KeyToRemove = {locale, key};
                        removeKey = true;
                        ImGui::CloseCurrentPopup();
                    }
                    else
                    {
                        ImGui::SameLine();
                        if(ImGui::Button("No"))
                        {
                            ImGui::CloseCurrentPopup();
                        }
                    }
                }
                for (auto& [variation, value] : values)
                {
                    ImGui::Text("    [");
                    ImGui::SameLine();
                    ImGui::InputText("##Variation", &variation);
                    ImGui::SameLine();
                    ImGui::Text("]: ");
                    ImGui::SameLine();
                    ImGui::InputText("##Value", &value);
                    if(values.size() > 1)
                    {
                        ImGui::SameLine();
                        if(ImGui::Button("-"))
                        {
                            ImGui::OpenPopup("Remove Variation");
                            ImGui::Text("Are you sure, that you want to delete variation %s?", variation.c_str());
                            if(ImGui::Button("Yes"))
                            {
                                m_ValueToRemove = {locale, key, variation};
                                removeValue = true;
                                ImGui::CloseCurrentPopup();
                            }
                            else
                            {
                                ImGui::SameLine();
                                if(ImGui::Button("No"))
                                {
                                    ImGui::CloseCurrentPopup();
                                }
                            }
                        }
                    }
                }
                if(ImGui::Button("+"))
                {
                    ImGui::OpenPopup("Add Variation");
                    ImGui::Text("Name");
                    ImGui::SameLine();
                    static std::array<char, 128> newVariation{'\0'};
                    ImGui::InputText("##New Variation", newVariation.data(), newVariation.size());
                    if(ImGui::Button("Add"))
                    {
                        values.emplace_back(newVariation.data(), "");
                        newVariation.fill('\0');
                        ImGui::CloseCurrentPopup();
                    }
                }
            }
        }
        if(removeValue)
        {
            auto& [locale, key, variation] = m_ValueToRemove;
            auto& keys = m_LocaleKeys[locale];
            auto it = std::find_if(keys.begin(), keys.end(), [&key](auto& pair){return pair.first == key;});
            if(it != keys.end())
            {
                auto& values = it->second;
                auto it2 = std::find_if(values.begin(), values.end(), [&variation](auto& pair){return pair.first == variation;});
                if(it2 != values.end())
                {
                    values.erase(it2);
                }
            }
            removeValue = false;
        }
        if(removeKey)
        {
            auto& [locale, key] = m_KeyToRemove;
            auto& keys = m_LocaleKeys[locale];
            auto it = std::find_if(keys.begin(), keys.end(), [&key](auto& pair){return pair.first == key;});
            if(it != keys.end())
            {
                keys.erase(it);
            }
            removeKey = false;
        }
        static bool addKeyPopup = false;
        // Добавление новых ключей и их локализаций
        if(ImGui::Button("Add Key"))
        {
            addKeyPopup = true;
        }
        if(addKeyPopup)
        {
            ImGui::OpenPopup("Add Key");
            ImGui::Text("Name");
            ImGui::SameLine();
            static std::array<char, 128> newKey{'\0'};
            ImGui::InputText("##New Key", newKey.data(), newKey.size());
            if(ImGui::Button("Add"))
            {
                m_LocaleKeys[m_SelectedLocale].emplace_back(newKey.data(), std::vector<std::pair<String, String>>{{"default", ""}});
                newKey.fill('\0');
                ImGui::CloseCurrentPopup();
                addKeyPopup = false;
            }
        }
        ImGui::SameLine();
        if(ImGui::Button("Save changes"))
        {
            for(auto& [locale, keys] : m_LocaleKeys)
            {
                for(auto& [key, values] : keys)
                {
                    for(auto& [variation, value] : values)
                    {
                        m_Domain->AddLocaleKey(locale, key, value, variation);
                    }
                }
                //Create all folders recursively if not created yet
                auto path = m_WorkingDirectory / "Localization" / (locale + ".yaml");
                if(!std::filesystem::exists(path.GetParent().ToStdPath()))
                {
                    std::filesystem::create_directories(path.GetParent().ToStdPath());
                }
                LocalizationGenerator::CreateLocalizationFile(*m_Domain, locale, path);
                if(std::ranges::find(m_Domain->m_LocalizationSources[locale], path) == m_Domain->m_LocalizationSources[locale].end())
                {
                    m_Domain->AddLocalizationSource(locale, path);
                }
            }
            m_Domain->Build();
        }
        ImGui::End();
    }

    void ImGuiLocalizationPanel::UpdateLocaleKeys()
    {
        m_LocaleKeys.clear();
        for (const auto& [locale, keyMap] : m_Domain->m_Languages)
        {
            for (const auto& [key, values] : keyMap)
            {
                std::vector<std::pair<String, String>> variants;
                variants.reserve(values.size());
                for (auto& [variation, value] : values)
                {
                    variants.emplace_back(variation, value);
                }
                m_LocaleKeys[locale].emplace_back(key, variants);
            }
        }
    }
}