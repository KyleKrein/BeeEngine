//
// Created by alexl on 17.10.2023.
//

#pragma once

#include "Core/Logging/Log.h"
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
} // namespace ImGui