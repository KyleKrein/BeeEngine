//
// Created by Александр Лебедев on 19.10.2023.
//

#include "ImGuiLocalizationPanel.h"
#include "LocalizationGenerator.h"
#include "Gui/ImGui/ImGuiExtension.h"
#include "Locale.h"
#include "Core/Color4.h"
#include <array>
#include <imgui_internal.h>
#include <algorithm>
namespace BeeEngine::Locale
{
    static float GetButtonWidth()
    {
        return ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.x * 2.0f;
    }
    ImGuiLocalizationPanel::ImGuiLocalizationPanel(Domain& domain, const Path& path)
    : m_Domain(&domain), m_SelectedLocale(domain.m_Locale), m_WorkingDirectory(path)
    {
        UpdateLocaleKeys();
    }
    void ImGuiLocalizationPanel::Render(ImGuiWindowFlags flags, bool canBeClosed)
    {
        if(canBeClosed)
        {
            if(!m_IsOpened)
                return;
            ImGui::Begin("Localization", &m_IsOpened, flags);
        }
        else
        {
            ImGui::Begin("Localization", nullptr, flags);
        }
        RenderUpperPanel();

        bool removeValue = false;
        bool removeKey = false;
        RenderKeysAndValues(removeValue, removeKey);
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
            for(auto&[ localeStr, keys] : m_LocaleKeys)
            {
                auto it = std::find_if(keys.begin(), keys.end(), [&key](auto& pair){return pair.first == key;});
                if(it != keys.end())
                {
                    keys.erase(it);
                }
            }
            removeKey = false;
        }
        // Добавление новых ключей и их локализаций
        if(ImGui::Button("Add Key"))
        {
            ImGui::OpenPopup("Add Key");
        }
        if(ImGui::BeginPopup("Add Key"))
        {
            ImGui::Text("Name");
            ImGui::SameLine();
            static std::string newKey;
            ImGui::InputText("##New Key 123", &newKey);
            static String errorMessage;
            if(!errorMessage.empty())
            {
                ImGui::TextColored(Color4::Red, errorMessage.c_str());
            }
            if(ImGui::Button("Add"))
            {
                if(newKey.empty())
                {
                    errorMessage = "Key name cannot be empty";
                    goto endPopup;
                }
                auto& keysVec = m_LocaleKeys[m_SelectedLocale];
                if(std::ranges::find_if(keysVec, [](auto& pair){return pair.first == newKey;}) != keysVec.end())
                {
                    errorMessage = "Key with this name already exists";
                    goto endPopup;
                }
                for(auto& [locale, keys] : m_LocaleKeys)
                {
                    keys.emplace_back(newKey.data(), std::vector<std::pair<String, String>>{{"default", ""}});
                }
                newKey = "";
                errorMessage = "";
                ImGui::CloseCurrentPopup();
            }
            endPopup:
            ImGui::EndPopup();
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
                auto path = m_WorkingDirectory / (locale + ".yaml");
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

    void ImGuiLocalizationPanel::RenderKeysAndValues(bool &removeValue, bool &removeKey)
    {// Отображение текущих ключей и их локализаций
        ImGui::BeginChild("##Lower", {-1, ImGui::GetContentRegionAvail().y - 60}, false);
        {
            auto &locale = m_SelectedLocale;
            auto &keys = m_LocaleKeys[locale];
            size_t i = 0;
            size_t j = 0;
            static std::vector<VariantToValuePair> *valuesPtr = nullptr;
            static size_t variationIndex = 0;
            ImGui::BeginTable("localization_table", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders);
            ImGui::TableSetupColumn("Keys");
            ImGui::TableSetupColumn("Variations");
            ImGui::TableSetupColumn("Values");
            ImGui::TableHeadersRow();
            for (auto &[key, values]: keys)
            {
                float buttonWidth = GetButtonWidth();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                float columnWidth = ImGui::GetColumnWidth();
                ImGui::PushItemWidth(columnWidth - buttonWidth - ImGui::GetStyle().ItemSpacing.x);
                String tempKey = key;
                if(ImGui::InputText(FormatString("##Key {}", i).c_str(), &tempKey, ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    {
                        auto it = std::ranges::find_if(keys, [&tempKey](auto &pair)
                        { return pair.first == tempKey; });
                        if (tempKey.empty() || it != keys.end())
                        {
                            ImGui::OpenPopup(FormatString("Error##{}", i).c_str());
                            goto popupError;
                        }
                    }
                    for(auto& [_, tempKeys] : m_LocaleKeys)
                    {
                        auto it = std::ranges::find_if(tempKeys, [&key](auto& pair){return pair.first == key;});
                        if(it != tempKeys.end())
                        {
                            it->first = tempKey;
                        }
                    }
                }
                popupError:
                if(ImGui::BeginPopup(FormatString("Error##{}", i).c_str()))
                {
                    ImGui::TextColored(Color4::Red,"Key name cannot be empty or already exist");
                    ImGui::EndPopup();
                }
                ImGui::PopItemWidth();
                ImGui::SameLine();
                if (ImGui::Button(/*"-"*/FormatString("-## {}", i).c_str(), {buttonWidth, buttonWidth}))
                {
                    ImGui::OpenPopup(FormatString("Remove Key {}", i).c_str());
                }
                if (ImGui::BeginPopup(FormatString("Remove Key {}", i).c_str()))
                {
                    ImGui::Text("Are you sure, that you want to delete key %s?", key.c_str());
                    if (ImGui::Button("Delete"))
                    {
                        m_KeyToRemove = {locale, key};
                        removeKey = true;
                        ImGui::CloseCurrentPopup();
                    } else
                    {
                        ImGui::SameLine();
                        if (ImGui::Button("Cancel"))
                        {
                            ImGui::CloseCurrentPopup();
                        }
                    }
                    ImGui::EndPopup();
                }
                size_t defaultVariationIndex = j;
                for (auto &[variation, value]: values)
                {
                    if (j != defaultVariationIndex) ImGui::TableNextRow(); // Создаем новую строку для каждой следующей пары "вариация-значение"\

                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushItemWidth(-1);
                    ImGui::InputText(FormatString("##Variation {}", j).c_str(), &variation);
                    ImGui::PopItemWidth();
                    ImGui::TableSetColumnIndex(2);
                    columnWidth = ImGui::GetColumnWidth();
                    ImGui::PushItemWidth(columnWidth -
                                         (buttonWidth + ImGui::GetStyle().ItemSpacing.x) * (values.size() > 1 ? 2 : 1));
                    ImGui::InputText(FormatString("##Value {}", j).c_str(), &value);
                    ImGui::PopItemWidth();
                    if (values.size() > 1)
                    {
                        ImGui::SameLine();
                        if (ImGui::Button(FormatString("-##{}", j).c_str(), {buttonWidth, buttonWidth}))
                        {
                            ImGui::OpenPopup(FormatString("Remove Variation {}", j).c_str());
                        }
                        if (ImGui::BeginPopup(FormatString("Remove Variation {}", j).c_str()))
                        {
                            ImGui::Text("Are you sure, that you want to delete variation %s?", variation.c_str());
                            if (ImGui::Button("Delete"))
                            {
                                m_ValueToRemove = {locale, key, variation};
                                removeValue = true;
                                ImGui::CloseCurrentPopup();
                            } else
                            {
                                ImGui::SameLine();
                                if (ImGui::Button("Cancel"))
                                {
                                    ImGui::CloseCurrentPopup();
                                }
                            }
                            ImGui::EndPopup();
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(FormatString("+##{}", j).c_str(), {buttonWidth, buttonWidth}))
                    {
                        ImGui::OpenPopup(FormatString("Add Variation {}", j).c_str());
                        valuesPtr = &values;
                        variationIndex = j;
                    }
                    if (ImGui::BeginPopup(FormatString("Add Variation {}", j).c_str()))
                    {
                        ImGui::Text("Variation");
                        ImGui::SameLine();
                        static std::array<char, 128> newVariation{'\0'};
                        ImGui::InputText("##New Variation", newVariation.data(), newVariation.size());
                        ImGui::Text("Value");
                        ImGui::SameLine();
                        static std::array<char, 128> newValue{'\0'};
                        ImGui::InputText("##New Value", newValue.data(), newValue.size());
                        if (ImGui::Button("Add"))
                        {
                            valuesPtr->emplace_back(newVariation.data(), newValue.data());
                            newVariation.fill('\0');
                            newValue.fill('\0');
                            ImGui::CloseCurrentPopup();
                        } else
                        {
                            ImGui::SameLine();
                            if (ImGui::Button("Cancel"))
                            {
                                newVariation.fill('\0');
                                ImGui::CloseCurrentPopup();
                            }
                        }
                        ImGui::EndPopup();
                    }
                    j++;
                }
                i++;
            }
            ImGui::EndTable();
            ImGui::EndChild();
        }
    }

    void ImGuiLocalizationPanel::RenderUpperPanel()
    {
        ImVec2 fixedUpperChildSize = {-1, ImGui::GetFontSize() * 3};
        ImGui::BeginChild("##Upper", fixedUpperChildSize, false);
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
        }
        if(ImGui::BeginPopup("Add Locale", ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Name");
            ImGui::SameLine();
            static String newLocale;
            static bool isIncorrectLocale = false;
            ImGui::InputText("##New Locale", &newLocale);
            if(ImGui::Button("Add"))
            {
                if(newLocale.empty())
                {
                    ImGui::OpenPopup("Error");
                    isIncorrectLocale = true;
                    goto endPopup;
                }
                if(m_LocaleKeys.contains(newLocale))
                {
                    ImGui::OpenPopup("Error");
                    isIncorrectLocale = true;
                    goto endPopup;
                }
                isIncorrectLocale = false;
                m_LocaleKeys.insert({newLocale.data(), {}});
                auto& newLocaleKeys = m_LocaleKeys[newLocale.data()];
                for(auto& [key, values] : m_LocaleKeys[m_SelectedLocale])
                {
                    newLocaleKeys.emplace_back(key, decltype(values){{"default", ""}});
                }
                newLocale = "";
                ImGui::CloseCurrentPopup();
            }
            endPopup:
            if(isIncorrectLocale)
            {
                ImGui::TextColored(Color4::Red, newLocale.empty() ? "Locale name cannot be empty" : FormatString("Locale with name {} already exists", newLocale).c_str());
            }
            ImGui::EndPopup();
        }
        if(locales.size() > 1)
        {
            ImGui::SameLine();
            if(ImGui::Button("-"))
            {
                ImGui::OpenPopup("Remove Locale");
            }
            if(ImGui::BeginPopup("Remove Locale"))
            {
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
                ImGui::EndPopup();
            }
        }
        ImGui::SameLine();
        float buttonSize = GetButtonWidth();
        //ImVec2 buttonCoords = {ImGui::GetContentRegionAvail().x - buttonSize, 0};
        //ImGui::SetCursorPos(buttonCoords);
        if(ImGui::Button("?", {buttonSize, buttonSize}))
        {
            ImGui::OpenPopup("Help##LocalizationPanel");
        }
        if(ImGui::BeginPopup("Help##LocalizationPanel"))
        {
            ImGui::Text("Базовое использование:");
            ImGui::BulletText("Подставлять переменные: {variableName}");
            ImGui::BulletText("Пример: \"Привет, {name}!\" где name - имя пользователя.");

            ImGui::Spacing();
            ImGui::Text("Плюрализация:");
            ImGui::BulletText("Общий синтаксис: {variableName, plural, one{...} other{...}}");
            ImGui::BulletText("Пример: \"{apples, plural, one{У меня есть 1 яблоко} other{У меня есть # яблока/яблок}}\"");

            ImGui::Spacing();
            ImGui::Text("Выбор:");
            ImGui::BulletText("Общий синтаксис: {variableName, select, value1{...} value2{...} other{...}}");
            ImGui::BulletText("Пример: \"{gender, select, male{Он пошёл домой} female{Она пошла домой} other{Они пошли домой}}\"");

            ImGui::Spacing();
            ImGui::Text("Склонение:");
            ImGui::BulletText("ICU не предоставляет напрямую функции для склонений, но...");
            ImGui::BulletText("Пример: \"{gender, select, male{Мой друг {name}} female{Моя подруга {name}} other{Мой друг/Моя подруга {name}}}\"");
            ImGui::EndPopup();
        }
        ImGui::EndChild();
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