//
// Created by alexl on 04.06.2023.
//

#include "SceneHierarchyPanel.h"
#include "Scene/Components.h"


namespace BeeEngine::Editor
{

    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene> &context)
    {
        SetContext(context);
    }

    void SceneHierarchyPanel::SetContext(const Ref<Scene> &context)
    {
        m_Context = context;
    }

    void SceneHierarchyPanel::OnGUIRender() noexcept
    {
        ImGui::Begin("Scene Hierarchy");
        m_Context->m_Registry.each([&](auto entityID)
        {
            Entity entity {EntityID{entityID}, m_Context.get()};
            DrawEntityNode(entity);
        });

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        {
            m_SelectedEntity = Entity::Null;
        }

        if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
        {
            if (ImGui::MenuItem("Create Empty Entity"))
            {
                m_Context->CreateEntity();
            }
            if (ImGui::MenuItem("Create Camera"))
            {
                auto newCamera = m_Context->CreateEntity("Camera");
                newCamera.AddComponent<CameraComponent>();
            }
            ImGui::EndPopup();
        }
        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity) noexcept
    {
        auto& tag = entity.GetComponent<TagComponent>().Tag;

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
                ImGuiTreeNodeFlags_SpanAvailWidth;

        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

        if (ImGui::IsItemClicked())
        {
            m_SelectedEntity = entity;
        }

        bool entityDeleted = false;
        if(ImGui::BeginPopupContextItem())
        {
            if(ImGui::MenuItem("Delete Entity"))
            {
                entityDeleted = true;
            }
            if(ImGui::MenuItem("Duplicate Entity"))
            {
                m_Context->DuplicateEntity(entity);
            }
            ImGui::EndPopup();
        }

        if(opened)
        {
            ImGui::TreePop();
        }

        if(entityDeleted)
        {
            m_Context->DestroyEntity(entity);
            if(m_SelectedEntity == entity)
            {
                m_SelectedEntity = Entity::Null;
            }
        }
    }

    void SceneHierarchyPanel::ClearSelection()
    {
        m_SelectedEntity = Entity::Null;
    }

    void SceneHierarchyPanel::OnEvent(EventDispatcher &e) noexcept
    {
        DISPATCH_EVENT(e, KeyPressedEvent, EventType::KeyPressed, OnKeyPressedEvent);
    }

    bool SceneHierarchyPanel::OnKeyPressedEvent(KeyPressedEvent *e) noexcept
    {
        bool ctrl = Input::KeyPressed(Key::LeftControl) || Input::KeyPressed(Key::RightControl);
        if(ctrl && Input::KeyPressed(Key::D))
        {
            if(m_SelectedEntity)
            {
                m_Context->DuplicateEntity(m_SelectedEntity);
                return true;
            }
        }
        return false;
    }
}
