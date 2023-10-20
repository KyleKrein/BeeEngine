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

namespace BeeEngine::Locale
{
    class Domain;
    class ImGuiLocalizationPanel
    {
        using VariantToValuePair = std::pair<String, String>;
        using KeyToVariantsPair = std::pair<String, std::vector<VariantToValuePair>>;
        using LocaleToKeyMap = std::unordered_map<String, std::vector<KeyToVariantsPair>>;
    public:
        ImGuiLocalizationPanel(Domain& domain, const Path& path);
        void Render(ImGuiWindowFlags flags = ImGuiWindowFlags_None, bool canBeClosed = true);
        void SwitchOpened() { m_IsOpened = !m_IsOpened; }
        void UpdateLocaleKeys();
    private:
        Domain* m_Domain;
        Path m_WorkingDirectory;
        String m_SelectedLocale;
        LocaleToKeyMap m_LocaleKeys;
        std::tuple<String, String, String> m_ValueToRemove;
        std::pair<String, String> m_KeyToRemove;
        bool m_IsOpened = false;

        void RenderUpperPanel();

        void RenderKeysAndValues(bool &removeValue, bool &removeKey);
    };
}