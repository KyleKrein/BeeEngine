//
// Created by alexl on 11.11.2023.
//

#include "AssetEditPanel.h"
#include "Core/AssetManagement/AssetManager.h"
#include "Gui/ImGui/ImGuiExtension.h"

namespace BeeEngine::Editor
{

    AssetEditPanel::AssetEditPanel(Locale::Domain &domain, const AssetHandle& handle)
    : m_Handle(handle), m_Domain(&domain)
    {
        bool isLoaded = AssetManager::IsAssetLoaded(handle);
        auto& asset = AssetManager::GetAsset<Asset>(handle);
        m_AssetType = asset.GetType();
        m_AssetName = asset.Name;
        if(!isLoaded &&
        m_AssetType != AssetType::Texture2D && m_AssetType != AssetType::Font//TODO: remove this when I add thumbnails
        )
        {
            AssetManager::UnloadAsset(handle);
            m_NeedsToBeUnloaded = false;
        }
    }

    void AssetEditPanel::Render()
    {
        if(!m_IsOpened)
            return;
        ImGui::Begin(m_Domain->Translate("assetEditPanel").c_str(), &m_IsOpened);
        ImGui::Text(m_Domain->Translate("name", "name", m_AssetName.c_str()).c_str());
        ImGui::Text(m_Domain->Translate("type", "type", ToString(m_AssetType).c_str()).c_str());
        if(m_AssetType == AssetType::Localized)
        {
            RenderLocalizedAssetSettings();
            if(ImGui::Button(m_Domain->Translate("save").c_str()))
            {
                HandleLocalizedAssetSave();
            }
        }
        ImGui::End();
    }

    AssetEditPanel::~AssetEditPanel()
    {
        if(m_NeedsToBeUnloaded)
        {
            Application::SubmitToMainThread([handle = m_Handle]()
                                            {
                                                if(AssetManager::IsAssetLoaded(handle))
                                                    AssetManager::UnloadAsset(handle);
                                            });
        }
    }

    void AssetEditPanel::RenderLocalizedAssetSettings()
    {

    }

    void AssetEditPanel::HandleLocalizedAssetSave()
    {

    }
} // BeeEngine