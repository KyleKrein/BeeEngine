//
// Created by alexl on 04.06.2023.
//

#include "InspectorPanel.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "gtc/type_ptr.hpp"
#include "imgui_internal.h"

namespace BeeEngine::Editor
{

    InspectorPanel::InspectorPanel(const Ref<Scene> &context)
    {
        SetContext(context);
    }

    void InspectorPanel::SetContext(const Ref<Scene> &context)
    {
        m_Context = context;
    }

    void InspectorPanel::OnGUIRender(Entity selectedEntity) noexcept
    {
        ImGui::Begin("Inspector");

        if(selectedEntity == Entity::Null)
        {
            ImGui::End();
            return;
        }

        DrawComponents(selectedEntity);

        if(ImGui::Button("Add Component"))
        {
            ImGui::OpenPopup("AddComponent");

            if(ImGui::BeginPopup("AddComponent"))
            {
                if(ImGui::MenuItem("Camera"))
                {
                    selectedEntity.AddComponent<CameraComponent>();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        ImGui::End();
    }

    void InspectorPanel::DrawComponents(Entity entity)
    {
        if(entity.HasComponent<TagComponent>())
        {
            auto& tag = entity.GetComponent<TagComponent>().Tag;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
#ifdef WINDOWS
            strcpy_s(buffer, sizeof(buffer), tag.c_str());
#else
            strcpy(buffer, tag.c_str());
#endif

            ImGui::Text("Tag");
            if(ImGui::InputText("##", buffer, sizeof(buffer)))
            {
                tag = std::string(buffer);
            }
        }
        if (entity.HasComponent<TransformComponent>())
        {
            if (ImGui::TreeNodeEx((void *) typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
            {
                auto& transform = entity.GetComponent<TransformComponent>();
                DrawVec3ComponentUI("Translation", transform.Translation);
                glm::vec3 rotation = glm::degrees(transform.Rotation);
                DrawVec3ComponentUI("Rotation", rotation);
                transform.Rotation = glm::radians(rotation);
                DrawVec3ComponentUI("Scale", transform.Scale, 1.0f);

                ImGui::TreePop();
            }
        }

        if (entity.HasComponent<CameraComponent>())
        {
            if (ImGui::TreeNodeEx((void *) typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
            {
                auto& camera = entity.GetComponent<CameraComponent>();
                constexpr static const char* projectionTypeStrings[] = {"Perspective", "Orthographic"};
                SceneCamera::CameraType currentProjectionType = camera.Camera.GetProjectionType();
                const char* currentProjectionTypeString = projectionTypeStrings[static_cast<int>(currentProjectionType)];
                if(ImGui::BeginCombo("Projection", currentProjectionTypeString))
                {
                    for(int i = 0; i < 2; ++i)
                    {
                        bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
                        if(ImGui::Selectable(projectionTypeStrings[i], isSelected))
                        {
                            currentProjectionTypeString = projectionTypeStrings[i];
                            camera.Camera.SetProjectionType(static_cast<SceneCamera::CameraType>(i));
                        }
                        if(isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                if(currentProjectionType == SceneCamera::CameraType::Perspective)
                {
                    float fov = glm::degrees(camera.Camera.GetPerspectiveVerticalFOV());
                    if(ImGui::DragFloat("FOV", &fov, 0.1f))
                    {
                        camera.Camera.SetPerspectiveVerticalFOV(glm::radians(fov));
                    }
                    float nearClip = camera.Camera.GetPerspectiveNearClip();
                    if(ImGui::DragFloat("Near Clip", &nearClip, 0.1f))
                    {
                        camera.Camera.SetPerspectiveNearClip(nearClip);
                    }
                    float farClip = camera.Camera.GetPerspectiveFarClip();
                    if(ImGui::DragFloat("Far Clip", &farClip, 0.1f))
                    {
                        camera.Camera.SetPerspectiveFarClip(farClip);
                    }
                }
                else if(camera.Camera.GetProjectionType() == SceneCamera::CameraType::Orthographic)
                {
                    float size = camera.Camera.GetOrthographicSize();
                    if(ImGui::DragFloat("Size", &size, 0.1f))
                    {
                        camera.Camera.SetOrthographicSize(size);
                    }
                    float nearClip = camera.Camera.GetOrthographicNearClip();
                    if(ImGui::DragFloat("Near Clip", &nearClip, 0.1f))
                    {
                        camera.Camera.SetOrthographicNearClip(nearClip);
                    }
                    float farClip = camera.Camera.GetOrthographicFarClip();
                    if(ImGui::DragFloat("Far Clip", &farClip, 0.1f))
                    {
                        camera.Camera.SetOrthographicFarClip(farClip);
                    }
                    ImGui::Checkbox("Fixed Aspect Ratio", &camera.FixedAspectRatio);
                    bool oldPrimary = camera.Primary;
                    ImGui::Checkbox("Primary", &camera.Primary);
                    if(camera.Primary != oldPrimary && camera.Primary)
                    {
                        auto cameras = m_Context->m_Registry.view<CameraComponent>();
                        for(auto cameraEntity : cameras)
                        {
                            m_Context->m_Registry.get<CameraComponent>(cameraEntity).Primary = false;
                        }
                        camera.Primary = true;
                    }
                }

                ImGui::TreePop();
            }
        }
    }

    void InspectorPanel::DrawVec3ComponentUI(std::string_view label, glm::vec3 &values, float resetValue,
                                             float columnWidth)
    {
        ImGui::PushID(label.data());
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.data());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

        ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};


        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
        if(ImGui::Button("X", buttonSize))
        {
            values.x = resetValue;
        }
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();


        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        if(ImGui::Button("Y", buttonSize))
        {
            values.y = resetValue;
        }
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();


        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.2f, 0.8f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.3f, 0.9f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.2f, 0.8f, 1.0f});
        if(ImGui::Button("Z", buttonSize))
        {
            values.x = resetValue;
        }
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);
        ImGui::PopID();
    }
}
