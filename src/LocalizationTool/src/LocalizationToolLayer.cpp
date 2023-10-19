//
// Created by alexl on 19.10.2023.
//

#include "LocalizationToolLayer.h"
#include "Utils/FileDialogs.h"
#include "Locale/LocalizationGenerator.h"


namespace BeeEngine::LocalizationTool
{

    void LocalizationToolLayer::OnAttach()
    {
        m_Domain.SetLocale("en_US");
        Path folder = FileDialogs::OpenFolder();
        auto paths = Locale::LocalizationGenerator::GetLocalizationFiles(folder);
        Locale::LocalizationGenerator::ProcessLocalizationFiles(m_Domain, paths);
        m_Domain.Build();
        m_LocalizationPanel = CreateRef<Locale::ImGuiLocalizationPanel>(m_Domain, folder);
        m_LocalizationPanel->SwitchOpened();
    }

    void LocalizationToolLayer::OnDetach()
    {

    }

    void LocalizationToolLayer::OnUpdate()
    {

    }

    void LocalizationToolLayer::OnGUIRendering()
    {
        m_LocalizationPanel->Render();
    }

    void LocalizationToolLayer::OnEvent(EventDispatcher &e)
    {

    }
}
