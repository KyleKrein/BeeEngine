//
// Created by alexl on 19.08.2023.
//

#include "AssetPanel.h"
#include "imgui.h"
#include "Renderer/Texture.h"
#include "Core/AssetManagement/AssetManager.h"
#include "Renderer/Font.h"
#include "Core/AssetManagement/EngineAssetRegistry.h"
#include "Core/Application.h"
#include "Core/ResourceManager.h"
#include "Core/Path.h"
#include "Gui/ImGui/ImGuiExtension.h"
#include "FileSystem/File.h"


namespace BeeEngine::Editor
{

    AssetPanel::AssetPanel(EditorAssetManager *assetManager, Locale::Domain& domain)
    : m_AssetManager(assetManager), m_EditorDomain(&domain)
    {

    }

    void AssetPanel::SetProject(ProjectFile* project)
    {
        m_Project = project;
    }

    void AssetPanel::Render()
    {
        if(m_AssetEditPanel)
        {
            m_AssetEditPanel->Render();
            if(!m_AssetEditPanel->IsOpened())
                m_AssetEditPanel.reset();
        }
        ImGui::Begin(m_EditorDomain->Translate("assetPanel").c_str());
        if(ImGui::IsDragAndDropPayloadInProcess("CONTENT_BROWSER_ITEM"))
        {
            auto width = ImGui::GetContentRegionAvail().x;
            ImGui::Button(m_EditorDomain->Translate("assetPanel.dropAssetsButton").c_str(), {width, 0});
            ImGui::AcceptDragAndDrop("CONTENT_BROWSER_ITEM", [this](void* data, size_t size)
                {
                    Path assetPath = m_Project->GetProjectPath() / (const char*)data;
                    if(ResourceManager::IsAssetExtension(assetPath.GetExtension()))
                    {
                        auto name = assetPath.GetFileNameWithoutExtension().AsUTF8();
                        auto* handlePtr = m_AssetManager->GetAssetHandleByName(name);
                        if(!handlePtr)
                        {
                            m_AssetManager->LoadAsset(assetPath, {m_Project->GetAssetRegistryID()});
                        }
                    }
                });
        }

        auto& registry = m_AssetManager->GetAssetRegistry();

        static float padding = 16.0f;
        static float thumbnailSize = 64.0f;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1)
            columnCount = 1;

        ImGui::Columns(columnCount, "AssetPanelColumns", false);

        for (auto& [registryID, registryMap] : registry)
        {
            for(auto& [assetID, metadata] : registryMap)
            {
                ImGui::PushID(metadata.Name.c_str());
                Texture2D* icon = nullptr;
                AssetHandle handle = {registryID, assetID};
                switch (metadata.Type)
                {
                    case AssetType::Texture2D:
                    {
                        icon = &AssetManager::GetAsset<Texture2D>(handle, m_Project->GetProjectLocaleDomain().GetLocale());
                        break;
                    }
                    case AssetType::Font:
                    {
                        icon = &AssetManager::GetAsset<Font>(handle, m_Project->GetProjectLocaleDomain().GetLocale()).GetAtlasTexture();
                        break;
                    }
                    default:
                    {
                        icon = &AssetManager::GetAsset<Texture2D>(EngineAssetRegistry::FileTexture, Locale::Localization::Default);
                        break;
                    }
                }
                ImGui::PushStyleColor(ImGuiCol_Button, {0, 0, 0, 0});
                // ImVec4 hoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
                //hoveredColor.w = 0.3f;
                //ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoveredColor);
                //ImVec4 activeColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
                //activeColor.w = 0.5f;
                //ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
                float aspectRatio = (float)icon->GetWidth() / icon->GetHeight();
                ImGui::ImageButton((ImTextureID) icon->GetRendererID(), {thumbnailSize, thumbnailSize / aspectRatio}, {0, 1}, {1, 0});

                if(handle.RegistryID == m_Project->GetAssetRegistryID())
                {
                    if(ImGui::BeginPopupContextItem("AssetPanelPopupMenu", ImGuiPopupFlags_MouseButtonRight))
                    {
                        if(ImGui::MenuItem(m_EditorDomain->Translate("assetPanel.editAsset").c_str()))
                        {
                            m_AssetEditPanel = CreateScope<AssetEditPanel>(*m_EditorDomain, handle, *m_AssetManager);
                        }
                        if(ImGui::MenuItem(m_EditorDomain->Translate("assetPanel.deleteAsset").c_str()))
                        {
                            Application::SubmitToMainThread([this, handle]()
                                                            {
                                                                m_OnAssetDeleted(handle);
                                                            });
                        }
                        ImGui::EndPopup();
                    }
                }

                ImGui::StartDragAndDrop(GetDragAndDropTypeName(metadata.Type), handle);

                ImGui::PopStyleColor();


                ImGui::TextWrapped(metadata.Name.c_str());

                ImGui::NextColumn();

                ImGui::PopID();
            }
        }

        ImGui::Columns(1);

        if (ImGui::BeginPopupContextWindow("AssetPanelPopupMenu", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
        {
            if (ImGui::MenuItem(m_EditorDomain->Translate("assetPanel.createLocalizedAsset").c_str()))
            {
                ImGui::OpenPopup("CreateLocalizedAssetPopup");
            }
            ImGui::EndPopup();
        }
        if(ImGui::BeginPopup("CreateLocalizedAssetPopup"))
        {
            static String assetName;
            ImGui::InputText(m_EditorDomain->Translate("name").c_str(), &assetName, 256);
            if(ImGui::Button(m_EditorDomain->Translate("create").c_str()))
            {
                AssetHandle handle = {};
                std::unordered_map<String, AssetHandle> handles;
                for(auto& locale : m_Project->GetProjectLocaleDomain().GetLocales())
                {
                    handles[locale] = {0,0};
                }
                LocalizedAsset localizedAsset {std::move(handles)};
                String serialized = LocalizedAssetSerializer::Serialize(localizedAsset);
                Path assetPath = m_Project->GetProjectPath() / (assetName + ".beelocalizedasset");
                File::WriteFile(assetPath, serialized);
                m_AssetEditPanel = CreateScope<AssetEditPanel>(*m_EditorDomain, handle, *m_AssetManager);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::End();
    }

    const char *AssetPanel::GetDragAndDropTypeName(AssetType type)
    {
        switch (type)
        {
            case AssetType::Texture2D:
                return "ASSET_BROWSER_TEXTURE2D_ITEM";
            case AssetType::Font:
                return "ASSET_BROWSER_FONT_ITEM";
            case AssetType::Prefab:
                return "ASSET_BROWSER_PREFAB_ITEM";
            case AssetType::MeshSource:
                return "ASSET_BROWSER_MESHSOURCE_ITEM";
            default:
                return "ASSET_BROWSER_UNKNOWN_ITEM";
        }
    }
}
