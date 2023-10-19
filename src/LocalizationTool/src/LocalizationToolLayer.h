//
// Created by alexl on 19.10.2023.
//

#pragma once

#include "Core/Layer.h"
#include "Locale/ImGuiLocalizationPanel.h"
#include "Locale/Locale.h"

namespace BeeEngine::LocalizationTool
{
    class LocalizationToolLayer: public Layer
    {
    public:
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate() override;
        void OnGUIRendering() override;
        void OnEvent(BeeEngine::EventDispatcher &e) override;
    private:
        Ref<Locale::ImGuiLocalizationPanel> m_LocalizationPanel;
        Locale::Domain m_Domain{"Localization"};
    };
}
