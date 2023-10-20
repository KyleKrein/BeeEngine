//
// Created by alexl on 19.10.2023.
//

#include "LocalizationToolLayer.h"
#include "Utils/FileDialogs.h"
#include "Locale/LocalizationGenerator.h"
#include <Gui/ImGui/ImGuiExtension.h>
#include "Core/Application.h"


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
    }

    void LocalizationToolLayer::OnDetach()
    {

    }

    void LocalizationToolLayer::OnUpdate()
    {

    }

    void LocalizationToolLayer::OnGUIRendering()
    {
        ImGuiWindowFlags window_flags = /*ImGuiWindowFlags_MenuBar |*/ ImGuiWindowFlags_NoDocking;
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        //ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        //ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 10.0f);
        window_flags |= /*ImGuiWindowFlags_NoTitleBar |*/ ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 0.0f));
        m_LocalizationPanel->Render(window_flags, false);
        //ImGui::PopStyleVar(2);
    }

    void LocalizationToolLayer::OnEvent(EventDispatcher &e)
    {

    }
}
