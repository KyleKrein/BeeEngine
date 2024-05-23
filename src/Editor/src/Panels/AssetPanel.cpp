//
// Created by alexl on 19.08.2023.
//

#include "AssetPanel.h"
#include "Core/Application.h"
#include "Core/AssetManagement/Asset.h"
#include "Core/AssetManagement/AssetManager.h"
#include "Core/AssetManagement/EngineAssetRegistry.h"
#include "Core/Path.h"
#include "Core/ResourceManager.h"
#include "FileSystem/File.h"
#include "Gui/ImGui/ImGuiExtension.h"
#include "Renderer/Font.h"
#include "Renderer/Texture.h"
#include "imgui.h"
#include "magic_enum.hpp"
#include <string_view>

namespace BeeEngine::Editor
{

    AssetPanel::AssetPanel(EditorAssetManager* assetManager, Locale::Domain& domain)
        : m_AssetManager(assetManager), m_EditorDomain(&domain)
    {
    }

    void AssetPanel::SetProject(ProjectFile* project)
    {
        m_Project = project;
    }

    void AssetPanel::Render()
    {
        if (m_AssetEditPanel)
        {
            m_AssetEditPanel->Render();
            if (!m_AssetEditPanel->IsOpened())
            {
                m_AssetEditPanel.reset();
            }
        }
        ImGui::Begin(m_EditorDomain->Translate("assetPanel").c_str());

        ImGui::BeginChild("##AssetPanelHeader", {0, 0}, ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY);

        ImGui::TextUnformatted(m_EditorDomain->Translate("filter").c_str());
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
        if (ImGui::InputText("##AssetPanelFilterName", &m_Filter.Name, ImGuiInputTextFlags_AutoSelectAll))
        {
            m_Filter.NameLower = ToLowercase(std::string_view(m_Filter.Name));
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::BeginCombo("##AssetPanelFilterType", magic_enum::enum_name(m_Filter.Type).data()))
        {
            for (const auto& [type, name] : magic_enum::enum_entries<AssetType>())
            {
                bool isSelected = type == m_Filter.Type;
                if (ImGui::Selectable(name.data(), isSelected))
                {
                    m_Filter.Type = type;
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::EndChild();

        ImGui::BeginChild("##AssetPanelContent", {0, ImGui::GetContentRegionAvail().y}, ImGuiChildFlags_None);

        auto& registry = m_AssetManager->GetAssetRegistry();

        static float padding = 16.0f;
        static float thumbnailSize = 64.0f;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1)
            columnCount = 1;

        ImGui::Columns(columnCount, "AssetPanelColumns", false);

        for (auto& asset : m_Filter.FilterAssets())
        {
            ImGui::PushID(asset.Name.data());
            Texture2D* icon = nullptr;
            switch (asset.GetType())
            {
                case AssetType::Texture2D:
                {
                    icon = &static_cast<Texture2D&>(asset);
                    break;
                }
                case AssetType::Font:
                {
                    icon = &static_cast<Font&>(asset).GetAtlasTexture();
                    break;
                }
                default:
                {
                    icon = &AssetManager::GetAsset<Texture2D>(EngineAssetRegistry::FileTexture,
                                                              Locale::Localization::Default);
                    break;
                }
            }
            ImGui::PushStyleColor(ImGuiCol_Button, {0, 0, 0, 0});
            // ImVec4 hoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
            // hoveredColor.w = 0.3f;
            // ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoveredColor);
            // ImVec4 activeColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
            // activeColor.w = 0.5f;
            // ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
            float aspectRatio = (float)icon->GetWidth() / icon->GetHeight();
            ImGui::ImageButton(
                (ImTextureID)icon->GetRendererID(), {thumbnailSize, thumbnailSize / aspectRatio}, {0, 1}, {1, 0});

            if (asset.Handle.RegistryID == m_Project->GetAssetRegistryID())
            {
                if (ImGui::BeginPopupContextItem("AssetPanelPopupMenu", ImGuiPopupFlags_MouseButtonRight))
                {
                    if (ImGui::MenuItem(m_EditorDomain->Translate("assetPanel.editAsset").c_str()))
                    {
                        m_AssetEditPanel = CreateScope<AssetEditPanel>(*m_EditorDomain, asset.Handle, *m_AssetManager);
                    }
                    if (ImGui::MenuItem(m_EditorDomain->Translate("assetPanel.deleteAsset").c_str()))
                    {
                        Application::SubmitToMainThread([this, handle = asset.Handle]() { m_OnAssetDeleted(handle); });
                    }
                    ImGui::EndPopup();
                }
            }

            ImGui::StartDragAndDrop(GetDragAndDropTypeName(asset.GetType()), asset.Handle);

            ImGui::PopStyleColor();

            ImGui::TextWrapped(asset.Name.data());

            ImGui::NextColumn();

            ImGui::PopID();
        }

        ImGui::Columns(1);

        if (ImGui::BeginPopupContextWindow("AssetPanelPopupMenu",
                                           ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
        {
            if (ImGui::MenuItem(m_EditorDomain->Translate("assetPanel.createLocalizedAsset").c_str()))
            {
                ImGui::OpenPopup("CreateLocalizedAssetPopup");
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginPopup("CreateLocalizedAssetPopup"))
        {
            static String assetName;
            ImGui::InputText(m_EditorDomain->Translate("name").c_str(), &assetName, 256);
            if (ImGui::Button(m_EditorDomain->Translate("create").c_str()))
            {
                AssetHandle handle = {};
                std::unordered_map<String, AssetHandle> handles;
                for (auto& locale : m_Project->GetProjectLocaleDomain().GetLocales())
                {
                    handles[locale] = {0, 0};
                }
                LocalizedAsset localizedAsset{std::move(handles)};
                String serialized = LocalizedAssetSerializer::Serialize(localizedAsset);
                Path assetPath = m_Project->GetProjectPath() / (assetName + ".beelocalizedasset");
                File::WriteFile(assetPath, serialized);
                m_AssetEditPanel = CreateScope<AssetEditPanel>(*m_EditorDomain, handle, *m_AssetManager);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::EndChild();

        ImGui::End();
    }

    const char* AssetPanel::GetDragAndDropTypeName(AssetType type)
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
            case AssetType::Scene:
                return "ASSET_BROWSER_SCENE_ITEM";
            default:
                return "ASSET_BROWSER_UNKNOWN_ITEM";
        }
    }
} // namespace BeeEngine::Editor
