//
// Created by alexl on 04.06.2023.
//

#include "SceneHierarchyPanel.h"
#include "Core/DeletionQueue.h"
#include "Core/Input.h"
#include "Gui/ImGui/ImGuiExtension.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Prefab.h"

namespace BeeEngine::Editor
{

    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
    {
        SetContext(context);
    }

    void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
    {
        m_Context = context;
    }

    void SceneHierarchyPanel::Update()
    {
        if (m_SelectedEntity != Entity::Null && !m_SelectedEntity.IsValid())
        {
            ClearSelection();
        }
    }

    void SceneHierarchyPanel::OnGUIRender() noexcept
    {
        ImGui::Begin(m_EditorDomain->Translate("sceneHierarchyPanel").c_str());
        if (ImGui::IsDragAndDropPayloadInProcess("ENTITY_ID") ||
            ImGui::IsDragAndDropPayloadInProcess("ASSET_BROWSER_PREFAB_ITEM"))
        {
            auto width = ImGui::GetContentRegionAvail().x;
            ImGui::Button(m_EditorDomain->Translate("sceneHierarchyPanel.toTopLevel").c_str(), {width, 0});
            ImGui::AcceptDragAndDrop<entt::entity>("ENTITY_ID",
                                                   [this](auto& e) mutable
                                                   {
                                                       Entity droppedEntity = {e, m_Context};
                                                       droppedEntity.RemoveParent();
                                                   });
            ImGui::AcceptDragAndDrop<AssetHandle>("ASSET_BROWSER_PREFAB_ITEM",
                                                  [this](auto& e) mutable
                                                  {
                                                      Prefab& prefab = AssetManager::GetAsset<Prefab>(e);
                                                      m_Context->InstantiatePrefab(prefab, Entity::Null);
                                                  });
        }

        m_Context->m_Registry.view<HierarchyComponent>().each(
            [&](auto entityID, auto& hierarchy)
            {
                if (hierarchy.Parent == Entity::Null) // Только для "главных" сущностей
                {
                    DrawEntityNode({entityID, m_Context});
                }
            });

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        {
            m_SelectedEntity = Entity::Null;
        }

        if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
        {
            if (ImGui::MenuItem(m_EditorDomain->Translate("sceneHierarchyPanel.createEmptyEntity").c_str()))
            {
                m_Context->CreateEntity();
            }
            if (ImGui::MenuItem(m_EditorDomain->Translate("sceneHierarchyPanel.createCamera").c_str()))
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
        auto& hierarchy = entity.GetComponent<HierarchyComponent>(); // Предположим, что у всех есть этот компонент

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                   ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
                                   ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s", tag.c_str());
        entt::entity entityID = entity;
        ImGui::StartDragAndDrop("ENTITY_ID", entityID);
        ImGui::AcceptDragAndDrop<entt::entity>("ENTITY_ID",
                                               [this, currentEntity = entity](auto& e) mutable
                                               {
                                                   Entity droppedEntity = {e, m_Context};
                                                   droppedEntity.SetParent(currentEntity);
                                               });
        ImGui::AcceptDragAndDrop<AssetHandle>("ASSET_BROWSER_PREFAB_ITEM",
                                              [this, currentEntity = entity](auto& e) mutable
                                              {
                                                  Prefab& prefab = AssetManager::GetAsset<Prefab>(e);
                                                  m_Context->InstantiatePrefab(prefab, currentEntity);
                                              });
        if (ImGui::IsItemClicked())
        {
            m_SelectedEntity = entity;
        }

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem(m_EditorDomain->Translate("sceneHierarchyPanel.deleteEntity").c_str()))
            {
                DeletionQueue::Frame().PushFunction(
                    [this, entityToDelete = entity]() mutable
                    {
                        if (m_SelectedEntity == entityToDelete || entityToDelete.HasChild(m_SelectedEntity))
                        {
                            m_SelectedEntity = Entity::Null;
                        }
                        m_Context->DestroyEntity(entityToDelete);
                    });
            }
            if (ImGui::MenuItem(m_EditorDomain->Translate("sceneHierarchyPanel.duplicateEntity").c_str()))
            {
                DeletionQueue::Frame().PushFunction([this, entityToDublicate = entity]() mutable
                                                    { m_Context->DuplicateEntity(entityToDublicate); });
            }
            ImGui::EndPopup();
        }

        if (opened)
        {
            for (auto child : hierarchy.Children)
            {
                DrawEntityNode(child); // Рекурсивный вызов
            }
            ImGui::TreePop();
        }
    }

    void SceneHierarchyPanel::ClearSelection()
    {
        m_SelectedEntity = Entity::Null;
    }

    void SceneHierarchyPanel::OnEvent(EventDispatcher& e) noexcept
    {
        e.Dispatch<KeyPressedEvent>([this](auto& e) -> bool { return OnKeyPressedEvent(&e); });
    }

    bool SceneHierarchyPanel::OnKeyPressedEvent(KeyPressedEvent* e) noexcept
    {
        bool ctrl = Input::KeyPressed(Key::LeftControl) || Input::KeyPressed(Key::RightControl);
        if (ctrl && Input::KeyPressed(Key::D))
        {
            if (m_SelectedEntity)
            {
                m_Context->DuplicateEntity(m_SelectedEntity);
                return true;
            }
        }
        return false;
    }
} // namespace BeeEngine::Editor
