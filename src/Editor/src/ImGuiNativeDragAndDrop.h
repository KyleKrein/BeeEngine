//
// Created by alexl on 24.11.2023.
//

#pragma once
#include "Core/Path.h"
#include <vector>
#include "Core/Events/Event.h"
#include "Core/Events/EventImplementations.h"
#include "Gui/ImGui/ImGuiExtension.h"

namespace BeeEngine
{
    class ImGuiNativeDragAndDrop
    {
    public:
        void ImGuiRender()
        {
            if(m_Dragging)
            {
                if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceExtern))
                {
                    static auto ptr = &m_Files;
                    ImGui::SetDragDropPayload("EXTERN_DRAG_AND_DROP", &ptr, sizeof(void*));
                    ImGui::EndDragDropSource();
                }
            }
            //m_Dragging = false;
        }
        void OnEvent(EventDispatcher& dispatcher)
        {
            dispatcher.Dispatch<FileDragEnterEvent>([this](FileDragEnterEvent& event) -> bool
            {
                m_Dragging = true;
                auto& io = ImGui::GetIO();
                io.AddMouseButtonEvent(ImGuiMouseButton_Left, true);
                //io.MousePos = ImVec2(pEvent.GetX(), pEvent.GetY());
                return false;
            });
            dispatcher.Dispatch<FileDragEndEvent>([this](auto& event) -> bool
            {
                m_Dragging = false;
                auto& io = ImGui::GetIO();
                io.AddMouseButtonEvent(ImGuiMouseButton_Left, false);
                return false;
            });
            dispatcher.Dispatch<FileDragLeaveEvent>([this](auto& event) -> bool
            {
                m_Dragging = false;
                auto& io = ImGui::GetIO();
                io.AddMouseButtonEvent(ImGuiMouseButton_Left, false);
                return false;
            });
            dispatcher.Dispatch<FileDragEvent>([this](FileDragEvent& event) -> bool
            {
                return OnFileDragEvent(event);
            });
            dispatcher.Dispatch<FileDropEvent>([this](FileDropEvent& event) -> bool
            {
                return OnFileDropEvent(event);
            });
        }

    private:

        bool OnFileDropEvent(FileDropEvent& pEvent)
        {
            m_Files.clear();
            auto& io = ImGui::GetIO();
            io.AddMouseButtonEvent(ImGuiMouseButton_Left, false);
            for(auto& path : pEvent.GetPaths())
            {
                m_Files.push_back(path);
            }
            return false;
        }
        bool OnFileDragEvent(FileDragEvent& pEvent)
        {
            auto& io = ImGui::GetIO();
            io.MousePos = ImVec2(pEvent.GetX(), pEvent.GetY());
            return false;
        }
        bool m_Dragging = false;
        std::vector<Path> m_Files;
    };
} // BeeEngine
