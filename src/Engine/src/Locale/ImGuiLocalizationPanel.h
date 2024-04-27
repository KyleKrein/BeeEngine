//
// Created by Александр Лебедев on 19.10.2023.
//

#pragma once

#include "Core/String.h"
#include "Core/Path.h"
#include <unordered_map>
#include <vector>
#include <utility>
#include <Gui/ImGui/ImGuiExtension.h>
#include "Locale.h"

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
    private:
        Domain* m_Domain;
        Path m_WorkingDirectory;
        Localization m_SelectedLocale;
        LocaleToKeyMap m_LocaleKeys;
        std::pair<Localization, String> m_KeyToRemove;
        bool m_IsOpened = false;

        void RenderUpperPanel();

        void RenderKeysAndValues(bool &removeKey);
    };
}