//
// Created by Александр Лебедев on 19.10.2023.
//

#pragma once

#include "Core/Path.h"
#include "Core/String.h"
#include "Locale.h"
#include <Gui/ImGui/ImGuiExtension.h>
#include <unordered_map>
#include <utility>
#include <vector>

namespace BeeEngine::Locale
{
    class Domain;
    class ImGuiLocalizationPanel
    {
        using KeyToValuePair = std::pair<String, String>;
        using LocaleToKeyMap = std::unordered_map<String, std::vector<KeyToValuePair>>;

    public:
        ImGuiLocalizationPanel(Domain& domain, const Path& path);
        void Render(ImGuiWindowFlags flags = ImGuiWindowFlags_None, bool canBeClosed = true);
        void SwitchOpened() { m_IsOpened = !m_IsOpened; }
        void UpdateLocaleKeys();
        void RegenerateDomainInfo();

    private:
        Jobs::SpinLock m_DomainLock;
        Domain* m_Domain;
        Path m_WorkingDirectory;
        Localization m_SelectedLocale;
        LocaleToKeyMap m_LocaleKeys;
        std::pair<Localization, String> m_KeyToRemove;
        std::unordered_map<Localization, Path> m_LocaleFiles;
        std::vector<Localization> m_LocalesToDelete;
        bool m_IsOpened = false;

        String searchField;
        String searchFieldLowerCase;

        String newKey;
        String errorMessage;
        std::vector<String>* valuesPtr = nullptr;
        size_t variationIndex = 0;
        String newLocale;
        bool isIncorrectLocale = false;

        void RenderUpperPanel();

        void RenderKeysAndValues(bool& removeKey);
    };
} // namespace BeeEngine::Locale
