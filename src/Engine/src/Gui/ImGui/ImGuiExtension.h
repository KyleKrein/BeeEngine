//
// Created by alexl on 17.10.2023.
//

#pragma once

#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>
#include <functional>
#include "Core/Logging/Log.h"

namespace ImGui
{
    template<typename PayloadType>
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
    template<typename PayloadType>
    inline void AcceptDragAndDrop(const char* payloadName, const std::function<void(const PayloadType&)>& onAccept)
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadName))
            {
                //BeeCoreAssert(payload->DataSize == sizeof(PayloadType), "Wrong payload size");
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
}