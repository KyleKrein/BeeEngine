//
// Created by alexl on 07.06.2023.
//
#include "ContentBrowserPanel.h"
#include "BeeEngine.h"
#include "Core/AssetManagement/PrefabImporter.h"
#include "Core/ResourceManager.h"
#include "FileSystem/File.h"
#include "Gui/ImGui/ImGuiExtension.h"
#include "ImGuiNativeDragAndDrop.h"
#include "Renderer/Texture.h"
#include "Scene/SceneSerializer.h"
#include "imgui.h"

namespace BeeEngine::Editor
{
    static void OpenCreatePopup(const char* name,
                                bool open,
                                Locale::Domain* domain,
                                const std::function<void(const char*)>& onCreate) noexcept
    {
        if (open)
        {
            ImGui::OpenPopup(name);
        }

        if (ImGui::BeginPopup(name))
        {
            static std::array<char, 256> buffer;
            ImGui::Text("%s", domain->Translate("name").c_str());
            ImGui::InputText("##Name", buffer.data(), buffer.size());

            if (ImGui::Button(domain->Translate("create").c_str(), {120, 0}))
            {
                onCreate(buffer.data());
                ImGui::CloseCurrentPopup();
                buffer[0] = '\0';
            }

            ImGui::SameLine();
            if (ImGui::Button(domain->Translate("cancel").c_str(), {120, 0}))
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
        ImGui::BeginChild("##ContentBrowserPanel",
                          {0, 0},
                          ImGuiChildFlags_None | ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY);

        if (m_CurrentDirectory != m_WorkingDirectory)
        {
            if (ImGui::Button("<-"))
            {
                m_CurrentDirectory = m_CurrentDirectory.GetParent();
            }
            DragAndDropFileToFolder(m_CurrentDirectory.GetParent());
            ImGui::SameLine();
            ImGui::TextUnformatted(
                (m_WorkingDirectory.GetFileName() / m_CurrentDirectory.GetRelativePath(m_WorkingDirectory))
                    .AsCString());
        }
        else
        {
            ImGui::TextUnformatted(m_WorkingDirectory.GetFileName().AsCString());
        }
        ImGui::EndChild();
        ImGui::BeginChild("##ContentBrowserPanelContent", {0, ImGui::GetContentRegionAvail().y}, ImGuiChildFlags_None);
        if (ImGui::IsDragAndDropPayloadInProcess("ENTITY_ID"))
        {
            auto width = ImGui::GetContentRegionAvail().x;
            ImGui::Button(m_EditorDomain->Translate("contentBrowserPanel.exportPrefab").c_str(), {width, 0});
            ImGui::AcceptDragAndDrop<entt::entity>(
                "ENTITY_ID",
                [this](const auto& e)
                {
                    Entity droppedEntity = {e, m_Context};
                    BeeExpects(droppedEntity);
                    PrefabImporter::GeneratePrefab(droppedEntity,
                                                   m_CurrentDirectory /
                                                       (droppedEntity.GetComponent<TagComponent>().Tag + ".beeprefab"),
                                                   {m_Project->GetAssetRegistryID()});
                });
        }
        if (ImGui::IsDragAndDropPayloadInProcess("EXTERN_DRAG_AND_DROP"))
        {
            auto width = ImGui::GetContentRegionAvail().x;
            ImGui::Button(m_EditorDomain->Translate("contentBrowserPanel.copyFiles").c_str(), {width, 0});
            AcceptExternFilesAndCopy(m_CurrentDirectory);
        }

        static float padding = 18.0f;
        float thumbnailSize = m_Config.ThumbnailSize;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1)
        {
            columnCount = 1;
        }

        ImGui::Columns(columnCount, "retbsfbd", false);

        auto stdCurrentDirectory = m_CurrentDirectory.ToStdPath();

        for (auto& directoryEntry : std::filesystem::directory_iterator(stdCurrentDirectory))
        {
            const Path& path = directoryEntry.path();
            auto filename = path.GetFileName();
            auto extension = path.GetExtension();
            if (extension == ".csproj" || extension == ".sln" || filename == ".DS_Store" ||
                (File::IsDirectory(path) &&
                 (filename == "beeengine" || filename == ".vs" || filename == ".beeengine" || filename == ".idea")))
            {
                continue;
            }
            auto relativePath = path.GetRelativePath(m_WorkingDirectory);
            String filenameString = relativePath.GetFileName().AsUTF8();
            ImGui::PushID(filenameString.c_str());
            Ref<Texture2D>& icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
            ImGui::PushStyleColor(ImGuiCol_Button, {0, 0, 0, 0});
            // ImVec4 hoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
            // hoveredColor.w = 0.3f;
            // ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoveredColor);
            // ImVec4 activeColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
            // activeColor.w = 0.5f;
            // ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
            float aspectRatio = (float)icon->GetWidth() / (float)icon->GetHeight();
            ImGui::ImageButton(
                (ImTextureID)icon->GetRendererID(), {thumbnailSize, thumbnailSize / aspectRatio}, {0, 1}, {1, 0});

            ImGui::StartDragAndDrop(
                "CONTENT_BROWSER_ITEM", (void*)relativePath.AsCString(), relativePath.AsUTF8().size() + 1);

            if (File::IsDirectory(path))
            {
                DragAndDropFileToFolder(path);
            }

            ImGui::PopStyleColor();
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (directoryEntry.is_directory())
                {
                    m_CurrentDirectory /= path.GetFileName();
                }
            }
            bool openRenamePopup = false;
            bool openDeletePopup = false;
            if (ImGui::BeginPopupContextItem("ContentBrowserItemPopup", ImGuiPopupFlags_MouseButtonRight))
            {
                if (ImGui::MenuItem(m_EditorDomain->Translate("rename").c_str()))
                {
                    openRenamePopup = true;
                }
                if (ImGui::MenuItem(m_EditorDomain->Translate("delete").c_str()))
                {
                    openDeletePopup = true;
                }
                ImGui::EndPopup();
            }
            if (openRenamePopup)
            {
                ImGui::OpenPopup(m_EditorDomain->Translate("contentBrowserPanel.renamePopup").c_str());
            }
            if (openDeletePopup)
            {
                ImGui::OpenPopup(
                    m_EditorDomain->Translate("contentBrowserPanel.deletePopup", "name", path.GetFileName().AsCString())
                        .c_str());
            }
            if (ImGui::BeginPopupModal(m_EditorDomain->Translate("contentBrowserPanel.renamePopup").c_str()))
            {
                static String newName;
                ImGui::InputText("##NewName", &newName);
                if (ImGui::Button(m_EditorDomain->Translate("rename").c_str()))
                {
                    Application::SubmitToMainThread(
                        [path, newName = newName]()
                        {
                            std::error_code error;
                            auto newPath = path.GetParent() / (newName + path.GetExtension().AsUTF8());
                            std::filesystem::rename(path.ToStdPath(), newPath.ToStdPath(), error);
                            if (error)
                            {
                                BeeCoreError("Failed to rename file: {0}", error.message());
                            }
                        });
                    newName.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button(m_EditorDomain->Translate("cancel").c_str()))
                {
                    newName.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopupModal(
                    m_EditorDomain->Translate("contentBrowserPanel.deletePopup", "name", path.GetFileName().AsCString())
                        .c_str()))
            {
                if (ImGui::Button(m_EditorDomain->Translate("delete").c_str()))
                {
                    Application::SubmitToMainThread(
                        [path]()
                        {
                            std::error_code error;
                            if (File::IsDirectory(path))
                            {
                                std::filesystem::remove_all(path.ToStdPath(), error);
                            }
                            else
                            {
                                std::filesystem::remove(path.ToStdPath(), error);
                            }
                            if (error)
                            {
                                BeeCoreError("Failed to delete file: {0}", error.message());
                            }
                        });
                    std::error_code error;
                    if (File::IsDirectory(path))
                    {
                        std::filesystem::remove_all(path.ToStdPath(), error);
                    }
                    else
                    {
                        std::filesystem::remove(path.ToStdPath(), error);
                    }
                    if (error)
                    {
                        BeeCoreError("Failed to delete file: {0}", error.message());
                    }
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button(m_EditorDomain->Translate("cancel").c_str()))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            ImGui::TextWrapped("%s", filenameString.c_str());

            ImGui::NextColumn();

            ImGui::PopID();
        }

        ImGui::Columns(1);

        // TODO: status bar

        bool createScriptPopupOpen = false;
        bool createScenePopupOpen = false;
        bool createFolderPopupOpen = false;
        if (ImGui::BeginPopupContextWindow("##CreateMenu",
                                           ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
        {
            if (ImGui::MenuItem(m_EditorDomain->Translate("createFolder").c_str()))
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
        OpenCreatePopup(m_EditorDomain->Translate("createFolder").c_str(),
                        createFolderPopupOpen,
                        m_EditorDomain,
                        [&](const char* name)
                        {
                            auto path = m_CurrentDirectory / name;
                            File::CreateDirectory(path);
                        });
        OpenCreatePopup(m_EditorDomain->Translate("createScript").c_str(),
                        createScriptPopupOpen,
                        m_EditorDomain,
                        [&](const char* name)
                        {
                            String scriptName = name;
                            if (scriptName.empty())
                            {
                                scriptName = "NewScript";
                            }
                            Path scriptPath = m_CurrentDirectory / (scriptName + ".cs");
                            auto templ = ResourceManager::GetScriptTemplate(scriptName);
                            File::WriteFile(scriptPath, templ);
                            m_NeedToRegenerateSolution = true;
                        });
        OpenCreatePopup(m_EditorDomain->Translate("createScene").c_str(),
                        createScenePopupOpen,
                        m_EditorDomain,
                        [&](const char* name)
                        {
                            Ref<Scene> scene = CreateRef<Scene>();
                            SceneSerializer serializer(scene);
                            serializer.Serialize(m_CurrentDirectory / (String(name) + ".beescene"));
                        });
        ImGui::EndChild();
        /*
        ImVec2 fixedChildSize = {-1, ImGui::GetFontSize() * 3};
        ImGui::BeginChild("##ContentBrowserPanelSettings", fixedChildSize, ImGuiChildFlags_Border |
        ImGuiChildFlags_AutoResizeY); auto width = ImGui::GetContentRegionAvail().x;
        ImGui::TextUnformatted(m_EditorDomain->Translate("contentBrowserPanel.thumbnailSize").c_str());
        ImGui::SameLine();
        ImGui::SetNextItemWidth((width / 2) - (width - ImGui::GetContentRegionAvail().x));
        ImGui::SliderFloat("##thumbnailSize", &thumbnailSize, 16, 512);
        ImGui::SameLine();
        ImGui::TextUnformatted(m_EditorDomain->Translate("contentBrowserPanel.padding").c_str());
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::SliderFloat("##padding", &padding, 0, 32);
        ImGui::EndChild();
        */
        ImGui::End();
    }

    void ContentBrowserPanel::DragAndDropFileToFolder(const Path& path)
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                Path filePath = (const char*)payload->Data;

                if (!filePath.IsAbsolute())
                {
                    filePath = m_WorkingDirectory / filePath;
                }
                std::error_code error;
                std::filesystem::copy(filePath.ToStdPath(),
                                      (path / filePath.GetFileName()).ToStdPath(),
                                      std::filesystem::copy_options::recursive |
                                          std::filesystem::copy_options::overwrite_existing,
                                      error);
                if (!error)
                {
                    std::filesystem::remove_all(filePath.ToStdPath());
                    if (filePath.GetExtension() == ".cs")
                    {
                        m_NeedToRegenerateSolution = true;
                    }
                }
                else
                {
                    BeeCoreError("Failed to copy file: {0}", error.message());
                }
            }
            ImGui::EndDragDropTarget();
        }
        AcceptExternFilesAndCopy(path);
    }

    void ContentBrowserPanel::AcceptExternFilesAndCopy(const Path& folder) const
    {
        ImGui::AcceptDragAndDrop<std::vector<Path>*>(
            "EXTERN_DRAG_AND_DROP",
            [&folder, this](const auto& payload)
            {
                auto files = *payload;
                for (auto& path : files)
                {
                    std::error_code error;
                    std::filesystem::copy_file(path.ToStdPath(),
                                               (folder / path.GetFileName()).ToStdPath(),
                                               std::filesystem::copy_options::recursive |
                                                   std::filesystem::copy_options::overwrite_existing,
                                               error);
                    if (error)
                    {
                        BeeCoreError("Failed to copy file: {0}", error.message());
                    }
                }
            });
    }

    ContentBrowserPanel::ContentBrowserPanel(Property<Scope<ProjectFile>>& project,
                                             const Path& workingDirectory,
                                             Locale::Domain& editorDomain,
                                             const ConfigFile& config) noexcept
        : m_Config(config),
          m_WorkingDirectory(workingDirectory),
          m_CurrentDirectory(workingDirectory),
          m_EditorDomain(&editorDomain)
    {
        project.valueChanged().connect(
            [this](const auto& newProject)
            {
                m_Project = newProject.get();
                m_WorkingDirectory = newProject->FolderPath.get();
                m_CurrentDirectory = m_WorkingDirectory;
            });
        m_DirectoryIcon =
            AssetManager::GetAssetRef<Texture2D>(EngineAssetRegistry::DirectoryTexture, Locale::Localization::Default);
        m_FileIcon =
            AssetManager::GetAssetRef<Texture2D>(EngineAssetRegistry::FileTexture, Locale::Localization::Default);
    }
} // namespace BeeEngine::Editor
