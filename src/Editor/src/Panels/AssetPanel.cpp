//
// Created by alexl on 07.06.2023.
//
#include "../../../Engine/Assets/EmbeddedResources.h"
#include "AssetPanel.h"
#include "imgui.h"
#include "Renderer/Texture.h"
#include "BeeEngine.h"


namespace BeeEngine::Editor
{

    void AssetPanel::OnGUIRender() noexcept
    {
        ImGui::Begin("Content Browser");

        if (m_CurrentDirectory != std::filesystem::path(m_WorkingDirectory))
        {
            if (ImGui::Button("<-"))
            {
                m_CurrentDirectory = m_CurrentDirectory.parent_path();
            }
        }

        static float padding = 16.0f;
        static float thumbnailSize = 64.0f;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1)
            columnCount = 1;

        ImGui::Columns(columnCount, 0, false);

        for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
        {
            const auto& path = directoryEntry.path();
            auto relativePath = std::filesystem::relative(path, m_WorkingDirectory);
            std::string filenameString = relativePath.filename().string();
            ImGui::PushID(filenameString.c_str());
            Ref<Texture2D>& icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
            ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
            ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

            if (ImGui::BeginDragDropSource())
            {
                if(Application::GetOsPlatform() == OSPlatform::Windows)
                {
                    auto itemPath = relativePath.wstring();
                    ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath.c_str(), (itemPath.size() + 1) * sizeof(wchar_t));
                }
                else
                {
                    auto itemPath = relativePath.string();
                    ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath.c_str(), itemPath.size() + 1);
                }

                ImGui::EndDragDropSource();
            }

            ImGui::PopStyleColor();
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (directoryEntry.is_directory())
                {
                    m_CurrentDirectory /= path.filename();
                }
            }

            ImGui::TextWrapped(filenameString.c_str());

            ImGui::NextColumn();
            
            ImGui::PopID();
        }

        ImGui::Columns(1);

        //ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
        //ImGui::SliderFloat("Padding", &padding, 0, 32);

        // TODO: status bar

        ImGui::End();
    }

    AssetPanel::AssetPanel(const std::filesystem::path &workingDirectory) noexcept
            : m_WorkingDirectory(workingDirectory)
            , m_CurrentDirectory(workingDirectory)
    {
        using namespace BeeEngine::Internal;
        m_DirectoryIcon = Texture2D::CreateFromMemory(GetEmbeddedResource(EmbeddedResource::DirectoryTexture));
        m_FileIcon = Texture2D::CreateFromMemory(GetEmbeddedResource(EmbeddedResource::FileTexture));
    }
}
