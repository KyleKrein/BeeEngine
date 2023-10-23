//
// Created by alexl on 07.06.2023.
//
#include "ContentBrowserPanel.h"
#include "imgui.h"
#include "Renderer/Texture.h"
#include "BeeEngine.h"
#include "Core/ResourceManager.h"
#include "FileSystem/File.h"
#include "Scene/SceneSerializer.h"
#include "Core/AssetManagement//PrefabImporter.h"
#include "Gui/ImGui/ImGuiExtension.h"


namespace BeeEngine::Editor
{
    static void OpenCreatePopup(const char* name, bool open, Locale::Domain* domain, const std::function<void(const char*)>& onCreate) noexcept
    {
        if (open)
        {
            ImGui::OpenPopup(name);
        }

        if (ImGui::BeginPopup(name))
        {
            static std::array<char, 256> buffer;
            ImGui::Text(domain->Translate("name").c_str());
            ImGui::InputText("##Name", buffer.data(), buffer.size());

            if (ImGui::Button(domain->Translate("create").c_str(), { 120, 0 }))
            {
                onCreate(buffer.data());
                ImGui::CloseCurrentPopup();
                buffer[0] = '\0';
            }

            ImGui::SameLine();
            if (ImGui::Button(domain->Translate("cancel").c_str(), { 120, 0 }))
            {
                ImGui::CloseCurrentPopup();
                buffer[0] = '\0';
            }

            ImGui::EndPopup();
        }
    }

    void ContentBrowserPanel::OnGUIRender() noexcept
    {
        ImGui::Begin(m_EditorDomain->Translate("contentBrowserPanel").c_str());

        if (m_CurrentDirectory != m_WorkingDirectory)
        {
            if (ImGui::Button("<-"))
            {
                m_CurrentDirectory = m_CurrentDirectory.GetParent();
            }
            DragAndDropFileToFolder(m_CurrentDirectory.GetParent());
        }
        if(ImGui::IsDragAndDropPayloadInProcess("ENTITY_ID"))
        {
            auto width = ImGui::GetContentRegionAvail().x;
            ImGui::Button(m_EditorDomain->Translate("contentBrowserPanel.exportPrefab").c_str(), {width, 0});
            ImGui::AcceptDragAndDrop<entt::entity>("ENTITY_ID", [this](const auto& e){
                Entity droppedEntity = {e, m_Context.get()};
                BeeExpects(droppedEntity);
                PrefabImporter::GeneratePrefab(droppedEntity, m_CurrentDirectory / (droppedEntity.GetComponent<TagComponent>().Tag + ".beeprefab"), {m_Project->GetAssetRegistryID()});
            });
        }

        static float padding = 16.0f;
        static float thumbnailSize = 64.0f;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1)
            columnCount = 1;

        ImGui::Columns(columnCount, "retbsfbd", false);

        auto stdCurrentDirectory = m_CurrentDirectory.ToStdPath();

        for (auto& directoryEntry : std::filesystem::directory_iterator(stdCurrentDirectory))
        {
            const Path& path = directoryEntry.path();
            auto filename = path.GetFileName();
            auto extension = path.GetExtension();
            if(extension == ".csproj" || extension == ".sln" || filename == ".DS_Store" ||
            (File::IsDirectory(path) &&
            (filename == "beeengine" || filename == ".vs" || filename == ".beeengine" || filename == ".idea")))
                continue;
            auto relativePath = path.GetRelativePath( m_WorkingDirectory);
            String filenameString = relativePath.GetFileName().AsUTF8();
            ImGui::PushID(filenameString.c_str());
            Ref<Texture2D>& icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
            ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
           // ImVec4 hoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
            //hoveredColor.w = 0.3f;
            //ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoveredColor);
            //ImVec4 activeColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
            //activeColor.w = 0.5f;
            //ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
            ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

            ImGui::StartDragAndDrop("CONTENT_BROWSER_ITEM", (void *) relativePath.AsCString(), relativePath.AsUTF8().size() + 1);

            if(File::IsDirectory(path))
            {
                DragAndDropFileToFolder(path);
            }
            /*else if(ResourceManager::IsAssetExtension(extension))
            {
                if (ImGui::BeginPopupContextWindow("##AssetPopup", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
                {
                    if (ImGui::MenuItem("Load Asset"))
                    {
                    }
                    ImGui::EndPopup();
                }
            }*/

            ImGui::PopStyleColor();
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (directoryEntry.is_directory())
                {
                    m_CurrentDirectory /= path.GetFileName();
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

        bool createScriptPopupOpen = false;
        bool createScenePopupOpen = false;
        bool createFolderPopupOpen = false;
        if (ImGui::BeginPopupContextWindow("##CreateMenu", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
        {
            if(ImGui::MenuItem(m_EditorDomain->Translate("createFolder").c_str()))
            {
                createFolderPopupOpen = true;
            }
            if (ImGui::MenuItem(m_EditorDomain->Translate("createScript").c_str()))
            {
                createScriptPopupOpen = true;
            }
            if (ImGui::MenuItem(m_EditorDomain->Translate("createScene").c_str()))
            {
                createScenePopupOpen = true;
            }
            ImGui::EndPopup();
        }
        OpenCreatePopup(m_EditorDomain->Translate("createFolder").c_str(), createFolderPopupOpen, m_EditorDomain, [&](const char* name)
        {
            auto path = m_CurrentDirectory / name;
            File::CreateDirectory(path);
        });
        OpenCreatePopup(m_EditorDomain->Translate("createScript").c_str(), createScriptPopupOpen, m_EditorDomain, [&](const char* name)
        {
            std::string scriptName = name;
            if (scriptName.empty())
                scriptName = "NewScript";
            Path scriptPath = m_CurrentDirectory / (scriptName + ".cs");
            auto templ = ResourceManager::GetScriptTemplate(scriptName);
            File::WriteFile(scriptPath, templ);
            m_NeedToRegenerateSolution = true;
        });
        OpenCreatePopup(m_EditorDomain->Translate("createScene").c_str(), createScenePopupOpen, m_EditorDomain, [&](const char* name)
        {
            Ref<Scene> scene = CreateRef<Scene>();
            SceneSerializer serializer(scene);
            serializer.Serialize(m_CurrentDirectory / (String(name) + ".beescene"));
        });

        ImGui::End();
    }

    void ContentBrowserPanel::DragAndDropFileToFolder(const Path &path)
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                Path filePath = (const char*)payload->Data;

                if(!filePath.IsAbsolute())
                {
                    filePath = m_WorkingDirectory / filePath;
                }
                std::error_code error;
                if(std::filesystem::copy_file(filePath.ToStdPath(), (path / filePath.GetFileName()).ToStdPath(), std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing, error))
                {
                    std::filesystem::remove_all(filePath.ToStdPath());
                    if(filePath.GetExtension() == ".cs")
                    {
                        m_NeedToRegenerateSolution = true;
                    }
                }
                else
                    BeeCoreError("Failed to copy file: {0}", error.message());
            }
            ImGui::EndDragDropTarget();
        }
    }

    ContentBrowserPanel::ContentBrowserPanel(const Path &workingDirectory, Locale::Domain& editorDomain) noexcept
            : m_WorkingDirectory(workingDirectory)
            , m_CurrentDirectory(workingDirectory)
            , m_EditorDomain(&editorDomain)
    {
        m_DirectoryIcon = AssetManager::GetAssetRef<Texture2D>(EngineAssetRegistry::DirectoryTexture, "en_US");
        m_FileIcon = AssetManager::GetAssetRef<Texture2D>(EngineAssetRegistry::FileTexture, "en_US");
    }
}
