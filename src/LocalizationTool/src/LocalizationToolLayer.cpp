//
// Created by alexl on 19.10.2023.
//

#include "LocalizationToolLayer.h"
#include "Core/Application.h"
#include "Locale/LocalizationGenerator.h"
#include "Utils/FileDialogs.h"
#include <Core/AssetManagement/EngineAssetRegistry.h>
#include <Gui/ImGui/ImGuiExtension.h>
#include <mutex>

namespace BeeEngine::LocalizationTool
{
    const char* g_FileDialogKey = "Choose folder with localization files";
    void LocalizationToolLayer::OnAttach()
    {
        auto locale = Locale::GetSystemLocale();
        auto& fileIcon = AssetManager::GetAsset<Texture2D>(EngineAssetRegistry::FileTexture, locale);
        auto& folderIcon = AssetManager::GetAsset<Texture2D>(EngineAssetRegistry::DirectoryTexture, locale);
        ImGui::SetDefaultFileIcon(
            (ImTextureID)fileIcon.GetGPUResource().GetRendererID(), fileIcon.GetWidth(), fileIcon.GetHeight());
        ImGui::SetDefaultFolderIcon(
            (ImTextureID)folderIcon.GetGPUResource().GetRendererID(), folderIcon.GetWidth(), folderIcon.GetHeight());
        // ImGui::SetFileDialogThumbnailSize(m_Config.ThumbnailSize);
        ImGui::OpenFolderFileDialog(g_FileDialogKey);
        m_Domain.SetLocale(Locale::Localization::Default);
    }

    void LocalizationToolLayer::OnDetach() {}

    void LocalizationToolLayer::OnUpdate(FrameData& frameData) {}

    void LocalizationToolLayer::OnGUIRendering()
    {
        ImGuiWindowFlags window_flags = /*ImGuiWindowFlags_MenuBar |*/ ImGuiWindowFlags_NoDocking;
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        // ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 10.0f);
        window_flags |= /*ImGuiWindowFlags_NoTitleBar |*/ ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 0.0f));
        std::unique_lock lock(m_Lock);
        switch (m_State)
        {
            case State::FolderChoice:
            {
                if (ImGui::BeginFileDialog(g_FileDialogKey))
                {
                    if (ImGui::IsFileDialogReady())
                    {
                        auto chosenPath = ImGui::GetResultFileDialog();
                        if (chosenPath.has_value())
                        {
                            auto folder = BeeMove(chosenPath).value();
                            auto job = Jobs::CreateJob(
                                [this, folder = BeeMove(folder)]()
                                {
                                    auto paths = Locale::LocalizationGenerator::GetLocalizationFiles(folder);
                                    Locale::LocalizationGenerator::ProcessLocalizationFiles(m_Domain, paths);
                                    m_Domain.Build();
                                    std::unique_lock lock(m_Lock);
                                    m_State = State::Editor;

                                    m_LocalizationPanel = CreateRef<Locale::ImGuiLocalizationPanel>(m_Domain, folder);
                                });
                            Jobs::Schedule(BeeMove(job));
                            m_State = State::Waiting;
                        }
                        ImGui::CloseFileDialog();
                    }
                    ImGui::EndFileDialog();
                }
                break;
            }
            case State::Waiting:
                ImGui::Begin("##loading");
                ImGui::TextCentered("Loading. Please Wait...");
                ImGui::End();
                break;
            case State::Editor:
                m_LocalizationPanel->Render(window_flags, false);
                break;
        }

        // ImGui::PopStyleVar(2);
    }

    void LocalizationToolLayer::OnEvent(EventDispatcher& e) {}
} // namespace BeeEngine::LocalizationTool
