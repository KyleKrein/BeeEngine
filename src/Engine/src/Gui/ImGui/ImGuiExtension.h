//
// Created by alexl on 17.10.2023.
//

#pragma once

#include "Core/Application.h"
#include "Core/Logging/Log.h"
#include "Core/Path.h"
#include "Core/String.h"
#include "imgui.h"
#include <functional>
#include <misc/cpp/imgui_stdlib.h>

namespace ImGui
{
    template <typename PayloadType>
    inline void StartDragAndDrop(const char* payloadName, PayloadType& data)
    {
        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload(payloadName, &data, sizeof(PayloadType));
            ImGui::EndDragDropSource();
        }
    }
    inline void StartDragAndDrop(const char* payloadName, void* data, size_t size)
    {
        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload(payloadName, data, size);
            ImGui::EndDragDropSource();
        }
    }
    inline void AcceptDragAndDrop(const char* payloadName, const std::function<void(void*, size_t)>& onAccept)
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadName))
            {
                onAccept(payload->Data, payload->DataSize);
            }
            ImGui::EndDragDropTarget();
        }
    }
    template <typename PayloadType>
    inline void AcceptDragAndDrop(const char* payloadName, const std::function<void(const PayloadType&)>& onAccept)
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadName))
            {
                // BeeCoreAssert(payload->DataSize == sizeof(PayloadType), "Wrong payload size");
                const PayloadType& data = *(const PayloadType*)payload->Data;
                onAccept(data);
            }
            ImGui::EndDragDropTarget();
        }
    }
    inline bool IsDragAndDropPayloadInProcess(const char* payloadName)
    {
        return ImGui::GetDragDropPayload() && ImGui::GetDragDropPayload()->IsDataType(payloadName);
    }

    // ImGui::InputText() with String
    // Because text input needs dynamic resizing, we need to setup a callback to grow the capacity
    IMGUI_API bool InputText(const char* label,
                             ::BeeEngine::String* str,
                             ImGuiInputTextFlags flags = 0,
                             ImGuiInputTextCallback callback = nullptr,
                             void* user_data = nullptr);
    IMGUI_API bool InputTextMultiline(const char* label,
                                      ::BeeEngine::String* str,
                                      const ImVec2& size = ImVec2(0, 0),
                                      ImGuiInputTextFlags flags = 0,
                                      ImGuiInputTextCallback callback = nullptr,
                                      void* user_data = nullptr);
    IMGUI_API bool InputTextWithHint(const char* label,
                                     const char* hint,
                                     ::BeeEngine::String* str,
                                     ImGuiInputTextFlags flags = 0,
                                     ImGuiInputTextCallback callback = nullptr,
                                     void* user_data = nullptr);
    inline IMGUI_API void TextCentered(const char* text)
    {
        float win_width = ImGui::GetWindowSize().x;
        float text_width = ImGui::CalcTextSize(text).x;

        // calculate the indentation that centers the text on one line, relative
        // to window left, regardless of the `ImGuiStyleVar_WindowPadding` value
        float text_indentation = (win_width - text_width) * 0.5f;

        // if text is too long to be drawn on one line, `text_indentation` can
        // become too small or even negative, so we check a minimum indentation
        float min_indentation = 20.0f;
        if (text_indentation <= min_indentation)
        {
            text_indentation = min_indentation;
        }

        ImGui::SameLine(text_indentation);
        ImGui::PushTextWrapPos(win_width - text_indentation);
        ImGui::TextWrapped("%s", text);
        ImGui::PopTextWrapPos();
    }
    inline bool ButtonCentered(const char* label, float alignment = 0.5f)
    {
        ImGuiStyle& style = ImGui::GetStyle();

        float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 4.0f;
        float avail = ImGui::GetWindowSize().x;

        float off = (avail - size) * alignment;
        if (off > 0.0f)
        {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
        }

        return ImGui::Button(label);
    }
    inline void ImageCentered(ImTextureID user_texture_id,
                              const ImVec2& image_size,
                              float alignment = 0.5,
                              const ImVec2& uv0 = {0, 0},
                              const ImVec2& uv1 = {1, 1},
                              const ImVec4& tint_col = {1, 1, 1, 1},
                              const ImVec4& border_col = {0, 0, 0, 0})
    {
        ImGuiStyle& style = ImGui::GetStyle();

        float size = image_size.x + style.FramePadding.x * 2.0f + (border_col.w != 0 ? 2.0f: 0.0f);
        float avail = ImGui::GetWindowSize().x;

        float off = (avail - size) * alignment;
        if (off > 0.0f)
        {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
        }
        Image(user_texture_id, image_size, uv0, uv1, tint_col, border_col);
    }

    void SetDefaultFileIcon(ImTextureID icon, float width, float height);
    void SetDefaultFolderIcon(ImTextureID icon, float width, float height);
    void SetFileDialogThumbnailSize(float size);
    struct FileDialogTranslations
    {
        BeeEngine::String Rename = "Rename";
        BeeEngine::String Delete = "Delete";
        BeeEngine::String Cancel = "Cancel";
        BeeEngine::String Confirm = "Confirm";
        BeeEngine::String OpenFolder = "Open folder";
        BeeEngine::String CreateFolder = "Create folder";
        BeeEngine::String OpenFile = "Open file";
        BeeEngine::String Name = "Name";
        BeeEngine::String Create = "Create";

        BeeEngine::String RenamePopupTitle = "Choose a new name.";
        BeeEngine::String DeletePopupTitle = "Are you sure that you want to delete";
    };
    void SetFileDialogTranslations(FileDialogTranslations translations);
    const FileDialogTranslations& GetFileDialogTranslations();

    void OpenFileDialog(const char* key,
                        const char* filter,
                        bool multipleFiles = false,
                        const BeeEngine::Path& currentFolder =
                            (&BeeEngine::Application::GetInstance()
                                 ? BeeEngine::Application::GetInstance().Environment().HomeDirectory()
                                 : std::filesystem::current_path()));
    void OpenFileSaveDialog(const char* key,
                            const char* filter,
                            const BeeEngine::Path& currentFolder =
                                (&BeeEngine::Application::GetInstance()
                                     ? BeeEngine::Application::GetInstance().Environment().HomeDirectory()
                                     : std::filesystem::current_path()));
    void OpenFolderFileDialog(const char* key,
                              const BeeEngine::Path& currentFolder =
                                  (&BeeEngine::Application::GetInstance()
                                       ? BeeEngine::Application::GetInstance().Environment().HomeDirectory()
                                       : std::filesystem::current_path()));
    bool BeginFileDialog(const char* key);
    bool IsFileDialogReady();
    std::optional<BeeEngine::Path> GetResultFileDialog();
    void EndFileDialog();
    void CloseFileDialog();

} // namespace ImGui
