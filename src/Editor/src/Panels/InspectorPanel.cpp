//
// Created by alexl on 04.06.2023.
//

#include "InspectorPanel.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "gtc/type_ptr.hpp"
#include "imgui_internal.h"
#include "Gui/ImGuiFonts.h"

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

        ImGui::End();
    }

    void InspectorPanel::DrawComponents(Entity entity)
    {

        auto& tag = entity.GetComponent<TagComponent>().Tag;
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
#ifdef WINDOWS
        strcpy_s(buffer, sizeof(buffer), tag.c_str());
#else
        strcpy(buffer, tag.c_str());
#endif
        if(ImGui::InputText("##Tag", buffer, sizeof(buffer)))
        {
            tag = std::string(buffer);
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(-1);

        if(ImGui::Button("Add Component"))
        {
            ImGui::OpenPopup("AddComponent");
        }
        if(ImGui::BeginPopup("AddComponent"))
        {
            AddComponentPopup<CameraComponent>("Camera", entity);
            ImGui::EndPopup();
        }

        ImGui::PopItemWidth();


        DrawConsistentComponentUI<TransformComponent>("Transform", entity, [this](TransformComponent& transform)
        {
            DrawVec3ComponentUI("Translation", transform.Translation);
            glm::vec3 rotation = glm::degrees(transform.Rotation);
            DrawVec3ComponentUI("Rotation", rotation);
            transform.Rotation = glm::radians(rotation);
            DrawVec3ComponentUI("Scale", transform.Scale, 1.0f);
        });

        DrawComponentUI<CameraComponent>("Camera", entity, [this](CameraComponent& camera)
        {
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
        });
    }

    void InspectorPanel::DrawVec3ComponentUI(std::string_view label, glm::vec3 &values, float resetValue,
                                             float columnWidth)
    {
        ImGuiIO& io = ImGui::GetIO();

        auto boldFont = io.Fonts->Fonts[(int)ImGuiFonts::OpenSans_Bold];

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
        ImGui::PushFont(boldFont);
        if(ImGui::Button("X", buttonSize))
        {
            values.x = resetValue;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();


        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
        ImGui::PushFont(boldFont);
        if(ImGui::Button("Y", buttonSize))
        {
            values.y = resetValue;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();


        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.2f, 0.8f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.3f, 0.9f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.2f, 0.8f, 1.0f});
        ImGui::PushFont(boldFont);
        if(ImGui::Button("Z", buttonSize))
        {
            values.x = resetValue;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);
        ImGui::PopID();
    }

    template<typename T>
    void InspectorPanel::AddComponentPopup(std::string_view label, Entity entity)
    {
        if(ImGui::MenuItem(label.data()))
        {
            entity.AddComponent<CameraComponent>();
            ImGui::CloseCurrentPopup();
        }
    }

    template<typename T, typename UIFunction>
    void InspectorPanel::DrawConsistentComponentUI(std::string_view label, Entity entity, UIFunction func)
    {
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                                 ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
                                                 ImGuiTreeNodeFlags_FramePadding;

        if(m_Context->m_Registry.all_of<T>(entity))
        {
            auto& component = m_Context->m_Registry.get<T>(entity);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
            ImGui::Separator();
            bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, label.data());
            ImGui::PopStyleVar();
            if(open)
            {
                func(component);
                ImGui::TreePop();
            }
        }
    }

    template<typename T, typename UIFunction>
    void InspectorPanel::DrawComponentUI(std::string_view label, Entity entity, UIFunction func)
    {
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                                 ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
                                                 ImGuiTreeNodeFlags_FramePadding;

        if(m_Context->m_Registry.all_of<T>(entity))
        {
            auto& component = m_Context->m_Registry.get<T>(entity);

            ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImGui::Separator();
            bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, label.data());
            ImGui::PopStyleVar();
            ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
            if(ImGui::Button("...", ImVec2{lineHeight, lineHeight}))
            {
                ImGui::OpenPopup("ComponentSettings");
            }

            bool removeComponent = false;
            if(ImGui::BeginPopup("ComponentSettings"))
            {
                if(ImGui::MenuItem("Remove Component"))
                {
                    removeComponent = true;
                }
                ImGui::EndPopup();
            }

            if(open)
            {
                func(component);
                ImGui::TreePop();
            }

            if(removeComponent)
            {
                m_Context->m_Registry.remove<T>(entity);
            }
        }
    }
}