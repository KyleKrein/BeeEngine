#include "ImGuiExtension.h"
#include "Core/Application.h"
#include "Core/CodeSafety/Expects.h"
#include "Core/Format.h"
#include "Core/Logging/Log.h"
#include "Core/Move.h"
#include "Core/Path.h"
#include "Core/String.h"
#include "FileSystem/File.h"
#include "imgui.h"
#include <algorithm>
#include <cstddef>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace
{
    struct InputTextCallback_UserData
    {
        ::BeeEngine::String* Str;
        ImGuiInputTextCallback ChainCallback;
        void* ChainCallbackUserData;
    };
} // namespace

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
    InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        // Resize string callback
        // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back
        // to what we want.
        ::BeeEngine::String* str = user_data->Str;
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(data->BufTextLen);
        data->Buf = (char*)str->c_str();
    }
    else if (user_data->ChainCallback)
    {
        // Forward to user callback, if any
        data->UserData = user_data->ChainCallbackUserData;
        return user_data->ChainCallback(data);
    }
    return 0;
}

bool ImGui::InputText(const char* label,
                      ::BeeEngine::String* str,
                      ImGuiInputTextFlags flags,
                      ImGuiInputTextCallback callback,
                      void* user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
}

bool ImGui::InputTextMultiline(const char* label,
                               ::BeeEngine::String* str,
                               const ImVec2& size,
                               ImGuiInputTextFlags flags,
                               ImGuiInputTextCallback callback,
                               void* user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return InputTextMultiline(
        label, (char*)str->c_str(), str->capacity() + 1, size, flags, InputTextCallback, &cb_user_data);
}

bool ImGui::InputTextWithHint(const char* label,
                              const char* hint,
                              ::BeeEngine::String* str,
                              ImGuiInputTextFlags flags,
                              ImGuiInputTextCallback callback,
                              void* user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return InputTextWithHint(
        label, hint, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
}

namespace ImGui
{
    enum class FileDialogType
    {
        FileOpen,
        FileSave,
        FolderOpen
    };
    struct FileDialogData
    {
        FileDialogData(BeeEngine::String Key,
                       std::vector<BeeEngine::String> Filters,
                       bool MultipleFiles,
                       BeeEngine::Path CurrentFolder,
                       FileDialogType Type)
            : Key(BeeMove(Key)),
              Filters(BeeMove(Filters)),
              MultipleFiles(MultipleFiles),
              CurrentFolder(BeeMove(CurrentFolder)),
              Type(Type),
              InputFieldCurrentFolder(this->CurrentFolder)
        {
        }
        BeeEngine::String Key;
        std::vector<BeeEngine::String> Filters;
        bool MultipleFiles;
        BeeEngine::Path CurrentFolder;
        FileDialogType Type;
        std::optional<BeeEngine::Path> Result;
        bool Ready = false;
        BeeEngine::String InputFieldCurrentFolder;
        BeeEngine::String NewFileName;
        size_t SelectedFilter = 0;
    };

    std::unordered_map<BeeEngine::String, FileDialogData> g_FileDialogs;
    BeeEngine::String g_CurrentFileDialogKey;
    struct FileDialogImage
    {
        ImTextureID Icon = 0;
        float Width = 1;
        float Height = 1;
    };
    FileDialogImage g_DefaultFileIcon;
    FileDialogImage g_DefaultFolderIcon;
    float g_ThumbnailSize = 32;
    FileDialogTranslations g_FileDialogTranslations;

    void SetFileDialogThumbnailSize(float size)
    {
        g_ThumbnailSize = size;
    }
    void SetFileDialogTranslations(FileDialogTranslations translations)
    {
        g_FileDialogTranslations = BeeMove(translations);
    }
    const FileDialogTranslations& GetFileDialogTranslations()
    {
        return g_FileDialogTranslations;
    }

    void SetDefaultFileIcon(ImTextureID icon, float width, float height)
    {
        g_DefaultFileIcon = {icon, width, height};
    }
    void SetDefaultFolderIcon(ImTextureID icon, float width, float height)
    {
        g_DefaultFolderIcon = {icon, width, height};
    }
    static void DrawFileDialog();
    void OpenFileDialog(const char* key, const char* filter, bool multipleFiles, const BeeEngine::Path& currentFolder)
    {
        BeeExpects(key);
        BeeExpects(!g_FileDialogs.contains(key));
        BeeExpects(!currentFolder.IsEmpty() && BeeEngine::File::Exists(currentFolder) &&
                   BeeEngine::File::IsDirectory(currentFolder));
        if (!filter)
        {
            filter = "*";
        }
        auto splittedFilter = BeeEngine::SplitString(filter, " ");
        auto filters = std::vector<BeeEngine::String>(splittedFilter.begin(), splittedFilter.end());
        g_FileDialogs.emplace(
            key,
            FileDialogData(key,
                           filters,
                           multipleFiles,
                           currentFolder.IsAbsolute() ? currentFolder : currentFolder.GetAbsolutePath(),
                           FileDialogType::FileOpen));
    }
    void OpenFileSaveDialog(const char* key, const char* filter, const BeeEngine::Path& currentFolder)
    {
        BeeExpects(key && filter);
        BeeExpects(!g_FileDialogs.contains(key));
        BeeExpects(!currentFolder.IsEmpty() && BeeEngine::File::Exists(currentFolder) &&
                   BeeEngine::File::IsDirectory(currentFolder));
        auto splittedFilter = BeeEngine::SplitString(filter, " ");
        auto filters = std::vector<BeeEngine::String>(splittedFilter.begin(), splittedFilter.end());
        g_FileDialogs.emplace(
            key,
            FileDialogData(key,
                           filters,
                           false,
                           currentFolder.IsAbsolute() ? currentFolder : currentFolder.GetAbsolutePath(),
                           FileDialogType::FileSave));
    }
    void OpenFolderFileDialog(const char* key, const BeeEngine::Path& currentFolder)
    {
        BeeExpects(key);
        BeeExpects(!g_FileDialogs.contains(key));
        BeeExpects(!currentFolder.IsEmpty() && BeeEngine::File::Exists(currentFolder) &&
                   BeeEngine::File::IsDirectory(currentFolder));
        g_FileDialogs.emplace(
            key,
            FileDialogData(key,
                           {},
                           false,
                           currentFolder.IsAbsolute() ? currentFolder : currentFolder.GetAbsolutePath(),
                           FileDialogType::FolderOpen));
    }
    bool BeginFileDialog(const char* key)
    {
        BeeExpects(key);
        bool result = g_FileDialogs.contains(key);
        if (result)
        {
            g_CurrentFileDialogKey = key;
            ImGui::OpenPopup(key);
            result = ImGui::BeginPopupModal(key, nullptr, ImGuiWindowFlags_NoDocking);
            if (result)
            {
                // ImGui::Begin(key, nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_Modal);
                DrawFileDialog();
            }
        }
        return result;
    }
    bool IsFileDialogReady()
    {
        BeeExpects(!g_CurrentFileDialogKey.empty());
        BeeExpects(g_FileDialogs.contains(g_CurrentFileDialogKey));
        return g_FileDialogs.at(g_CurrentFileDialogKey).Ready;
    }
    std::optional<BeeEngine::Path> GetResultFileDialog()
    {
        BeeExpects(!g_CurrentFileDialogKey.empty());
        BeeExpects(g_FileDialogs.contains(g_CurrentFileDialogKey));
        BeeExpects(g_FileDialogs.at(g_CurrentFileDialogKey).Ready);
        auto result = std::move(g_FileDialogs.at(g_CurrentFileDialogKey).Result);
        g_FileDialogs.at(g_CurrentFileDialogKey).Result = {};
        return result;
    }
    void EndFileDialog()
    {
        BeeExpects(!g_CurrentFileDialogKey.empty());
        ImGui::EndPopup();
        g_CurrentFileDialogKey = "";
    }
    void CloseFileDialog()
    {
        BeeExpects(!g_CurrentFileDialogKey.empty() && g_FileDialogs.contains(g_CurrentFileDialogKey));
        g_FileDialogs.erase(g_CurrentFileDialogKey);
        ImGui::CloseCurrentPopup();
    }
    static void DragAndDropFileToFolder(const BeeEngine::Path& path, const BeeEngine::Path& workingDirectory)
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_DIALOG_ITEM"))
            {
                BeeEngine::Path filePath = (const char*)payload->Data;

                if (!filePath.IsAbsolute())
                {
                    filePath = workingDirectory / filePath;
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
                }
                else
                {
                    BeeCoreError("Failed to copy file: {0}", error.message());
                }
            }
            ImGui::EndDragDropTarget();
        }
    }
    static void DrawFileDialog()
    {
        auto& [key,
               filter,
               multipleFiles,
               currentFolder,
               type,
               result,
               ready,
               inputFieldCurrentFolder,
               newName,
               selectedFilterIndex] = g_FileDialogs.at(g_CurrentFileDialogKey);
        ImGui::BeginChild(
            "##FileDialogPanel", {0, 0}, ImGuiChildFlags_None | ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY);

        if (!currentFolder.IsEmpty())
        {
            if (ImGui::Button("<-"))
            {
                currentFolder = currentFolder.GetParent();
                inputFieldCurrentFolder = currentFolder;
                if (type == FileDialogType::FolderOpen)
                {
                    result = currentFolder;
                }
            }
            DragAndDropFileToFolder(currentFolder.GetParent(), currentFolder);
            ImGui::SameLine();
        }
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputText("##FileDialogFolder", &inputFieldCurrentFolder, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if (BeeEngine::File::Exists(inputFieldCurrentFolder))
            {
                if (!BeeEngine::File::IsDirectory(inputFieldCurrentFolder))
                {
                    inputFieldCurrentFolder = BeeEngine::Path{inputFieldCurrentFolder}.GetParent().AsUTF8();
                }
                currentFolder = inputFieldCurrentFolder;
                if (type == FileDialogType::FolderOpen)
                {
                    result = currentFolder;
                }
            }
            else
            {
                inputFieldCurrentFolder = currentFolder;
            }
        }
        ImGui::EndChild();
        ImGui::BeginChild(
            "##FileDialogPanelContent", {0, ImGui::GetContentRegionAvail().y - 120}, ImGuiChildFlags_None);

        static float padding = 18.0f;
        float thumbnailSize = g_ThumbnailSize;
        float cellSize = thumbnailSize + padding;

        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1)
        {
            columnCount = 1;
        }

        ImGui::Columns(columnCount, "retwqergqwgqwgeqwg", false);

        auto stdCurrentDirectory = currentFolder.ToStdPath();
        size_t fileCount = 0;

        for (auto& directoryEntry : std::filesystem::directory_iterator(stdCurrentDirectory))
        {
            fileCount++;
            const BeeEngine::Path path = directoryEntry.path();
            auto filename = path.GetFileName();
            auto extension = path.GetExtension();
            if (!BeeEngine::File::IsDirectory(path) &&
                (type == FileDialogType::FileOpen || type == FileDialogType::FileSave))
            {
                bool found = false;
                for (auto& filterEntry : filter)
                {
                    if (extension == filterEntry)
                    {
                        found = true;
                        break;
                    }
                    if (filterEntry == "*")
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    continue;
                }
            }
            auto relativePath = path.GetRelativePath(currentFolder);
            BeeEngine::String filenameString = relativePath.GetFileName().AsUTF8();
            ImGui::PushID(filenameString.c_str());
            auto icon = directoryEntry.is_directory() ? g_DefaultFolderIcon : g_DefaultFileIcon;
            ImGui::PushStyleColor(ImGuiCol_Button, {0, 0, 0, 0});
            // ImVec4 hoveredColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
            // hoveredColor.w = 0.3f;
            // ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoveredColor);
            // ImVec4 activeColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
            // activeColor.w = 0.5f;
            // ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
            float aspectRatio = icon.Width / icon.Height;

            ImGui::ImageButton(BeeEngine::FormatString("{0}", fileCount).c_str(),
                               icon.Icon,
                               {thumbnailSize, thumbnailSize / aspectRatio},
                               {0, 1},
                               {1, 0});

            ImGui::StartDragAndDrop(
                "FILE_DIALOG_ITEM", (void*)relativePath.AsCString(), relativePath.AsUTF8().size() + 1);

            if (BeeEngine::File::IsDirectory(path))
            {
                DragAndDropFileToFolder(path, currentFolder);
            }

            ImGui::PopStyleColor();
            if (ImGui::IsItemHovered())
            {
                if (directoryEntry.is_directory())
                {
                    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        currentFolder /= path.GetFileName();
                        inputFieldCurrentFolder = currentFolder;
                    }
                    if (type == FileDialogType::FolderOpen && (ImGui::IsMouseClicked(ImGuiMouseButton_Left) ||
                                                               ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)))
                    {
                        result = path;
                    }
                }
                else if (type == FileDialogType::FileOpen && (ImGui::IsMouseClicked(ImGuiMouseButton_Left) ||
                                                              ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)))
                {
                    result = path;
                    if (!multipleFiles && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        ready = true;
                    }
                }
            }
            bool openRenamePopup = false;
            bool openDeletePopup = false;
            if (ImGui::BeginPopupContextItem("FileDialogItemPopup", ImGuiPopupFlags_MouseButtonRight))
            {
                if (ImGui::MenuItem(g_FileDialogTranslations.Rename.c_str()))
                {
                    openRenamePopup = true;
                }
                if (ImGui::MenuItem(g_FileDialogTranslations.Delete.c_str()))
                {
                    openDeletePopup = true;
                }
                ImGui::EndPopup();
            }
            if (openRenamePopup)
            {
                ImGui::OpenPopup(g_FileDialogTranslations.RenamePopupTitle.c_str());
            }
            if (openDeletePopup)
            {
                ImGui::OpenPopup(
                    BeeEngine::FormatString("{0} {1}?", g_FileDialogTranslations.DeletePopupTitle, path.GetFileName())
                        .c_str());
            }
            if (ImGui::BeginPopupModal(g_FileDialogTranslations.RenamePopupTitle.c_str()))
            {
                static BeeEngine::String newName;
                ImGui::InputText("##NewName", &newName);
                if (ImGui::Button(g_FileDialogTranslations.Rename.c_str()))
                {
                    BeeEngine::Application::SubmitToMainThread(
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
                if (ImGui::Button(g_FileDialogTranslations.Cancel.c_str()))
                {
                    newName.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopupModal(
                    BeeEngine::FormatString("{0} {1}?", g_FileDialogTranslations.DeletePopupTitle, path.GetFileName())
                        .c_str()))
            {
                if (ImGui::Button(g_FileDialogTranslations.Delete.c_str()))
                {
                    BeeEngine::Application::SubmitToMainThread(
                        [path]()
                        {
                            std::error_code error;
                            if (BeeEngine::File::IsDirectory(path))
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
                    if (BeeEngine::File::IsDirectory(path))
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
                if (ImGui::Button(g_FileDialogTranslations.Cancel.c_str()))
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

        bool createFolderPopupOpen = false;
        if (ImGui::BeginPopupContextWindow("##CreateMenu",
                                           ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
        {
            if (ImGui::MenuItem(g_FileDialogTranslations.CreateFolder.c_str()))
            {
                createFolderPopupOpen = true;
            }
            ImGui::EndPopup();
        }
        if (createFolderPopupOpen)
        {
            ImGui::OpenPopup(g_FileDialogTranslations.CreateFolder.c_str());
        }

        if (ImGui::BeginPopup(g_FileDialogTranslations.CreateFolder.c_str()))
        {
            static std::array<char, 256> buffer;
            ImGui::Text("%s", g_FileDialogTranslations.Name.c_str());
            ImGui::InputText("##Name", buffer.data(), buffer.size());

            if (ImGui::Button(g_FileDialogTranslations.Create.c_str(), {120, 0}))
            {
                auto onCreate = [&](const char* name)
                {
                    auto path = currentFolder / name;
                    BeeEngine::File::CreateDirectory(path);
                };
                onCreate(buffer.data());
                ImGui::CloseCurrentPopup();
                buffer[0] = '\0';
            }

            ImGui::SameLine();
            if (ImGui::Button(g_FileDialogTranslations.Cancel.c_str(), {120, 0}))
            {
                ImGui::CloseCurrentPopup();
                buffer[0] = '\0';
            }

            ImGui::EndPopup();
        }
        ImGui::EndChild();

        ImGui::BeginChild("##FileDialogBottom", ImVec2(0, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_Border);
        const char* buttonText = nullptr;
        switch (type)
        {
            case FileDialogType::FileOpen:
                buttonText = g_FileDialogTranslations.OpenFile.c_str();
                break;
            case FileDialogType::FileSave:
                buttonText = g_FileDialogTranslations.Confirm.c_str();
                break;
            case FileDialogType::FolderOpen:
                buttonText = g_FileDialogTranslations.OpenFolder.c_str();
                break;
        }
        if (type != FileDialogType::FileSave)
        {
            ImGui::TextUnformatted(result.has_value() ? result.value().GetFileName().AsCString() : "");
        }
        else
        {
            bool hasFilters = std::find(filter.begin(), filter.end(), "*") == filter.end();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * (hasFilters ? 0.75f : 1.0f));
            if (ImGui::InputText("##FileNameFileDialog", &newName))
            {
                if (newName.empty())
                {
                    result = {};
                }
                else
                {
                    result = currentFolder / newName;
                    if (hasFilters &&
                        !result.value().AsUTF8().ends_with(static_cast<std::string_view>(filter[selectedFilterIndex])))
                    {
                        result.value().ReplaceExtension(filter[selectedFilterIndex]);
                    }
                }
            }
            if (hasFilters)
            {
                ImGui::SameLine();
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                if (ImGui::BeginCombo("##Filter", filter[selectedFilterIndex].c_str()))
                {
                    for (auto& filterStr : filter)
                    {
                        bool selected = filterStr == filter[selectedFilterIndex];
                        if (ImGui::Selectable(filterStr.c_str(), selected))
                        {
                            selectedFilterIndex =
                                std::distance(filter.begin(), std::find(filter.begin(), filter.end(), filterStr));
                        }
                        if (selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
            }
        }
        ImGui::BeginDisabled(!result.has_value());
        if (ImGui::Button(buttonText))
        {
            ready = true;
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::Button(g_FileDialogTranslations.Cancel.c_str()))
        {
            result.reset();
            ready = true;
        }
        ImGui::EndChild();
    }
} // namespace ImGui
