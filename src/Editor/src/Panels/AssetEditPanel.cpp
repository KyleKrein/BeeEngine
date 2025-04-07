//
// Created by alexl on 11.11.2023.
//

#include "AssetEditPanel.h"
#include "Core/AssetManagement/AssetManager.h"
#include "Gui/ImGui/ImGuiExtension.h"
#include "imgui.h"

namespace BeeEngine::Editor
{

    AssetEditPanel::AssetEditPanel(Locale::Domain& domain, const AssetHandle& handle, EditorAssetManager& assetManager)
        : m_Handle(handle), m_Domain(&domain), m_AssetManager(assetManager)
    {
    }

    void AssetEditPanel::Render()
    {
        if (!m_IsOpened)
            return;
        if (!m_AssetManager.IsAssetHandleValid(m_Handle))
        {
            m_IsOpened = false;
            return;
        }
        auto& metadata = m_AssetManager.GetAssetMetadata(m_Handle);
        ImGui::Begin(m_Domain->Translate("assetEditPanel").c_str(), &m_IsOpened);
        ImGui::TextUnformatted(m_Domain->Translate("assetEditPanel.name", "name", metadata.Name.c_str()).c_str());
        ImGui::TextUnformatted(
            m_Domain->Translate("assetEditPanel.type", "type", ToString(metadata.Type).c_str()).c_str());
        if (metadata.Type == AssetType::Localized)
        {
            RenderLocalizedAssetSettings();
            if (ImGui::Button(m_Domain->Translate("save").c_str()))
            {
                HandleLocalizedAssetSave();
            }
        }
        ImGui::End();
    }

    AssetEditPanel::~AssetEditPanel() {}

    void AssetEditPanel::RenderLocalizedAssetSettings() {}

    void AssetEditPanel::HandleLocalizedAssetSave() {}
} // namespace BeeEngine::Editor