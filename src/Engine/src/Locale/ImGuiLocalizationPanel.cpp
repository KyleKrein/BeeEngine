//
// Created by Александр Лебедев on 19.10.2023.
//

#include "ImGuiLocalizationPanel.h"
#include "Core/Color4.h"
#include "Core/String.h"
#include "Gui/ImGui/ImGuiExtension.h"
#include "Locale.h"
#include "LocalizationGenerator.h"
#include "imgui.h"
#include <Core/Move.h>
#include <algorithm>
#include <array>
#include <imgui_internal.h>
#include <string_view>
namespace BeeEngine::Locale
{
    static float GetButtonWidth()
    {
        return ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.x * 2.0f;
    }
    static Localization GetLocaleFromPath(const Path& path)
    {
        auto filename = path.GetFileName();
        auto locale = filename.AsUTF8().substr(0, filename.AsUTF8().find('.'));
        return {locale};
    }
    void ImGuiLocalizationPanel::RegenerateDomainInfo()
    {
        std::unique_lock lock(m_DomainLock);
        std::vector<Path> files = LocalizationGenerator::GetLocalizationFiles(m_WorkingDirectory);
        for (auto&& file : files)
        {
            auto loc = GetLocaleFromPath(file);
            m_LocaleFiles[loc] = BeeMoveAlways(file);
        }
        UpdateLocaleKeys();
    }
    ImGuiLocalizationPanel::ImGuiLocalizationPanel(Domain& domain, const Path& path)
        : m_Domain(&domain), m_SelectedLocale(domain.m_Locale), m_WorkingDirectory(path)
    {
        RegenerateDomainInfo();
    }
    void ImGuiLocalizationPanel::Render(ImGuiWindowFlags flags, bool canBeClosed)
    {
        if (canBeClosed)
        {
            if (!m_IsOpened)
                return;
            ImGui::Begin("Localization", &m_IsOpened, flags);
        }
        else
        {
            ImGui::Begin("Localization", nullptr, flags);
        }
        std::unique_lock lock(m_DomainLock);
        RenderUpperPanel();

        bool removeKey = false;
        RenderKeysAndValues(removeKey);
        if (removeKey)
        {
            auto& [locale, key] = m_KeyToRemove;
            for (auto& [localeStr, keys] : m_LocaleKeys)
            {
                auto it = std::find_if(keys.begin(), keys.end(), [&key](auto& pair) { return pair.first == key; });
                if (it != keys.end())
                {
                    keys.erase(it);
                }
            }
            removeKey = false;
        }
        // Добавление новых ключей и их локализаций
        if (ImGui::Button("Add Key"))
        {
            ImGui::OpenPopup("Add Key");
        }
        if (ImGui::BeginPopup("Add Key"))
        {
            ImGui::Text("Name");
            ImGui::SameLine();

            ImGui::InputText("##New Key 123", &newKey);
            if (!errorMessage.empty())
            {
                ImGui::TextColored(Color4::Red, "%s", errorMessage.c_str());
            }
            if (ImGui::Button("Add"))
            {
                if (newKey.empty())
                {
                    errorMessage = "Key name cannot be empty";
                    goto endPopup;
                }
                auto& keysVec = m_LocaleKeys[m_SelectedLocale.GetLanguageString()];
                if (std::ranges::find_if(keysVec, [this](auto& pair) { return pair.first == newKey; }) != keysVec.end())
                {
                    errorMessage = "Key with this name already exists";
                    goto endPopup;
                }
                for (auto& [locale, keys] : m_LocaleKeys)
                {
                    keys.emplace_back(newKey, "");
                }
                newKey = "";
                errorMessage = "";
                ImGui::CloseCurrentPopup();
            }
        endPopup:
            ImGui::EndPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Save changes"))
        {
            for (auto& [locale, keys] : m_LocaleKeys)
            {
                Localization loc = {locale};
                m_Domain->AddLocale(loc);
                for (auto& [key, value] : keys)
                {
                    m_Domain->AddLocaleKey(loc, key, value);
                }
                // Create all folders recursively if not created yet
                auto path = m_WorkingDirectory / (locale + ".yaml");
                if (!std::filesystem::exists(path.GetParent().ToStdPath()))
                {
                    std::filesystem::create_directories(path.GetParent().ToStdPath());
                }
                LocalizationGenerator::CreateLocalizationFile(*m_Domain, loc, path);
                if (std::ranges::find(m_Domain->m_LocalizationSources[loc], path) ==
                    m_Domain->m_LocalizationSources[locale].end())
                {
                    m_Domain->AddLocalizationSource(loc, path);
                    m_LocaleFiles[loc] = path;
                }
            }
            for (const auto& locale : m_LocalesToDelete)
            {
                m_Domain->m_LocalizationSources.erase(locale);
                m_Domain->m_Languages.erase(locale.GetLanguageString());
                if (m_LocaleFiles.contains(locale) && std::filesystem::exists(m_LocaleFiles[locale].ToStdPath()))
                {
                    std::filesystem::remove(m_LocaleFiles[locale].ToStdPath());
                    m_LocaleFiles.erase(locale);
                }
                if (m_Domain->GetLocale() == locale)
                {
                    m_Domain->SetLocale(m_Domain->m_Languages.begin()->first);
                }
            }
            m_LocalesToDelete.clear();
            m_Domain->Build();
        }
        ImGui::End();
    }

    void ImGuiLocalizationPanel::RenderKeysAndValues(bool& removeKey)
    { // Отображение текущих ключей и их локализаций
        ImGui::BeginChild("##Lower", {-1, ImGui::GetContentRegionAvail().y - 60}, false);
        {
            auto& locale = m_SelectedLocale.GetLanguageString();
            auto& keys = m_LocaleKeys[locale];
            size_t i = 0;
            ImGui::BeginTable("localization_table", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders);
            ImGui::TableSetupColumn("Keys");
            ImGui::TableSetupColumn("Value");
            ImGui::TableHeadersRow();
            for (auto& [key, value] : keys)
            {
                if (!searchField.empty() && !ToLowercase(std::string_view(key)).contains(searchFieldLowerCase) &&
                    !ToLowercase(std::string_view(value)).contains(searchFieldLowerCase))
                {
                    continue;
                }
                float buttonWidth = GetButtonWidth();
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                float columnWidth = ImGui::GetColumnWidth();
                ImGui::PushItemWidth(columnWidth /*- ImGui::GetStyle().ItemSpacing.x*/);
                String tempKey = key;
                if (ImGui::InputText(
                        FormatString("##Key {}", i).c_str(), &tempKey, ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    {
                        auto it = std::ranges::find_if(keys, [&tempKey](auto& pair) { return pair.first == tempKey; });
                        if (tempKey.empty() || it != keys.end())
                        {
                            ImGui::OpenPopup(FormatString("Error##{}", i).c_str());
                            goto popupError;
                        }
                    }
                    for (auto& [_, tempKeys] : m_LocaleKeys)
                    {
                        auto it = std::ranges::find_if(tempKeys, [&key](auto& pair) { return pair.first == key; });
                        if (it != tempKeys.end())
                        {
                            it->first = tempKey;
                        }
                    }
                }
            popupError:
                if (ImGui::BeginPopup(FormatString("Error##{}", i).c_str()))
                {
                    ImGui::TextColored(Color4::Red, "Key name cannot be empty or already exist");
                    ImGui::EndPopup();
                }
                ImGui::PopItemWidth();
                ImGui::TableSetColumnIndex(1);
                columnWidth = ImGui::GetColumnWidth();
                ImGui::PushItemWidth(columnWidth - (buttonWidth + ImGui::GetStyle().ItemSpacing.x));
                ImGui::InputText(FormatString("##Value {}", i).c_str(), &value);
                ImGui::SameLine();
                if (ImGui::Button(/*"-"*/ FormatString("-## {}", i).c_str(), {buttonWidth, buttonWidth}))
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
                    }
                    else
                    {
                        ImGui::SameLine();
                        if (ImGui::Button("Cancel"))
                        {
                            ImGui::CloseCurrentPopup();
                        }
                    }
                    ImGui::EndPopup();
                }
                ImGui::PopItemWidth();
                i++;
            }
            ImGui::EndTable();
            ImGui::EndChild();
        }
    }

    void ImGuiLocalizationPanel::RenderUpperPanel()
    {
        ImVec2 fixedUpperChildSize = {-1, ImGui::GetFontSize() * 3};
        ImGui::BeginChild("##Upper", fixedUpperChildSize, ImGuiChildFlags_None);
        auto& locales = m_LocaleKeys;
        // Верхний переключатель локализаций
        if (ImGui::BeginCombo("Locales", m_SelectedLocale.GetLanguageString().c_str()))
        {
            for (auto& [locale, keys] : locales)
            {
                bool isSelected = (m_SelectedLocale.GetLanguageString() == locale);
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
        if (ImGui::Button("+"))
        {
            ImGui::OpenPopup("Add Locale");
        }
        if (ImGui::BeginPopup("Add Locale", ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Name");
            ImGui::SameLine();
            ImGui::InputText("##New Locale", &newLocale);
            if (ImGui::Button("Add"))
            {
                if (newLocale.empty())
                {
                    ImGui::OpenPopup("Error");
                    isIncorrectLocale = true;
                    goto endPopup;
                }
                if (m_LocaleKeys.contains(newLocale))
                {
                    ImGui::OpenPopup("Error");
                    isIncorrectLocale = true;
                    goto endPopup;
                }
                if (newLocale.size() != 2)
                {
                    ImGui::OpenPopup("Error");
                    isIncorrectLocale = true;
                    goto endPopup;
                }
                isIncorrectLocale = false;
                m_LocaleKeys.insert({newLocale, {}});
                auto& newLocaleKeys = m_LocaleKeys[newLocale];
                if (m_LocaleKeys.size() > 1)
                {
                    for (auto& [locale, keys] : m_LocaleKeys)
                    {
                        if (locale != newLocale)
                        {
                            for (auto& [key, value] : keys)
                            {
                                newLocaleKeys.emplace_back(key, "");
                            }
                            break;
                        }
                    }
                }
                newLocale = "";
                ImGui::CloseCurrentPopup();
            }
        endPopup:
            if (isIncorrectLocale)
            {
                ImGui::TextColored(Color4::Red,
                                   "%s",
                                   newLocale.empty() ? "Locale name cannot be empty"
                                   : newLocale.size() != 2
                                       ? "Locale name must be 2 characters long"
                                       : FormatString("Locale with name {} already exists", newLocale).c_str());
            }
            ImGui::EndPopup();
        }
        if (locales.size() > 1)
        {
            ImGui::SameLine();
            if (ImGui::Button("-"))
            {
                ImGui::OpenPopup("Remove Locale");
            }
            if (ImGui::BeginPopup("Remove Locale"))
            {
                ImGui::Text("Are you sure, that you want to delete locale %s?",
                            m_SelectedLocale.GetLanguageString().c_str());
                if (ImGui::Button("Yes"))
                {
                    m_LocalesToDelete.push_back(m_SelectedLocale);
                    m_LocaleKeys.erase(m_SelectedLocale.GetLanguageString());
                    m_SelectedLocale = m_LocaleKeys.begin()->first;
                    ImGui::CloseCurrentPopup();
                }
                else
                {
                    ImGui::SameLine();
                    if (ImGui::Button("No"))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                }
                ImGui::EndPopup();
            }
        }
        ImGui::SameLine();
        float buttonSize = GetButtonWidth();
        // ImVec2 buttonCoords = {ImGui::GetContentRegionAvail().x - buttonSize, 0};
        // ImGui::SetCursorPos(buttonCoords);
        if (ImGui::Button("?", {buttonSize, buttonSize}))
        {
            ImGui::OpenPopup("Help##LocalizationPanel");
        }
        if (ImGui::BeginPopup("Help##LocalizationPanel"))
        {
            ImGui::Text("Базовое использование:");
            ImGui::BulletText("Подставлять переменные: {variableName}");
            ImGui::BulletText("Пример: \"Привет, {name}!\" где name - имя пользователя.");

            ImGui::Spacing();
            ImGui::Text("Плюрализация:");
            ImGui::BulletText("Общий синтаксис: {variableName, plural, one{...} other{...}}");
            ImGui::BulletText(
                "Пример: \"{apples, plural, one{У меня есть 1 яблоко} other{У меня есть # яблока/яблок}}\"");

            ImGui::Spacing();
            ImGui::Text("Выбор:");
            ImGui::BulletText("Общий синтаксис: {variableName, select, value1{...} value2{...} other{...}}");
            ImGui::BulletText(
                "Пример: \"{gender, select, male{Он пошёл домой} female{Она пошла домой} other{Они пошли домой}}\"");

            ImGui::Spacing();
            ImGui::Text("Склонение:");
            ImGui::BulletText("ICU не предоставляет напрямую функции для склонений, но...");
            ImGui::BulletText("Пример: \"{gender, select, male{Мой друг {name}} female{Моя подруга {name}} other{Мой "
                              "друг/Моя подруга {name}}}\"");
            ImGui::EndPopup();
        }

        ImGui::TextUnformatted("Search: ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputText("##Search", &searchField, ImGuiInputTextFlags_AutoSelectAll))
        {
            searchFieldLowerCase = ToLowercase(std::string_view(searchField));
        }

        ImGui::EndChild();
    }

    void ImGuiLocalizationPanel::UpdateLocaleKeys()
    {
        m_LocaleKeys.clear();
        for (const auto& [locale, keyMap] : m_Domain->m_Languages)
        {
            for (const auto& [key, value] : keyMap)
            {
                m_LocaleKeys[locale].emplace_back(key, value);
            }
        }
    }
} // namespace BeeEngine::Locale
