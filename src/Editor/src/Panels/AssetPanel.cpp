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


namespace BeeEngine::Editor
{

    AssetPanel::AssetPanel(EditorAssetManager *assetManager)
    : m_AssetManager(assetManager)
    {

    }

    void AssetPanel::SetProject(ProjectFile* project)
    {
        m_Project = project;
    }

    void AssetPanel::Render()
    {
        ImGui::Begin("Assets");
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                std::filesystem::path assetPath;
                if(Application::GetOsPlatform() == OSPlatform::Windows)
                {
                    assetPath = std::filesystem::path(m_Project->GetProjectPath()) / (const wchar_t*)payload->Data;
                }
                else
                {
                    assetPath = std::filesystem::path(m_Project->GetProjectPath()) / (const char*)payload->Data;
                }
                if(ResourceManager::IsAssetExtension(assetPath.extension()))
                {
                    auto name = ResourceManager::GetNameFromFilePath(assetPath.string());
                    auto* handlePtr = m_AssetManager->GetAssetHandleByName(name);
                    if(!handlePtr)
                    {
                        m_AssetManager->LoadAsset(assetPath, {m_Project->GetAssetRegistryID()});
                    }
                }
            }
            ImGui::EndDragDropTarget();
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
                        icon = &AssetManager::GetAsset<Texture2D>(handle);
                        break;
                    }
                    case AssetType::Font:
                    {
                        icon = &AssetManager::GetAsset<Font>(handle).GetAtlasTexture();
                        break;
                    }
                    default:
                    {
                        icon = &AssetManager::GetAsset<Texture2D>(EngineAssetRegistry::FileTexture);
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
                ImGui::ImageButton((ImTextureID) icon->GetRendererID(), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});

                if (ImGui::BeginDragDropSource())
                {
                    const char* name = GetDragAndDropTypeName(metadata.Type);
                    ImGui::SetDragDropPayload(name, &handle, sizeof(AssetHandle));

                    ImGui::EndDragDropSource();
                }

                ImGui::PopStyleColor();


                ImGui::TextWrapped(metadata.Name.c_str());

                ImGui::NextColumn();

                ImGui::PopID();
            }
        }

        ImGui::Columns(1);
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
            default:
                return "ASSET_BROWSER_UNKNOWN_ITEM";
        }
    }
}
