//
// Created by alexl on 04.06.2023.
//

#include "InspectorPanel.h"
#include "Core/AssetManagement/Asset.h"
#include "Core/AssetManagement/LocalizedAsset.h"
#include "Core/ResourceManager.h"
#include "Gui/ImGui/ImGuiExtension.h"
#include "Gui/ImGuiFonts.h"
#include "Panels/AssetPanel.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scripting/GameScript.h"
#include "Scripting/MClass.h"
#include "Scripting/ScriptingEngine.h"
#include "gtc/type_ptr.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <misc/cpp/imgui_stdlib.h>
#include <ranges>

#include "Scripting/NativeToManaged.h"

namespace BeeEngine::Editor
{

    InspectorPanel::InspectorPanel(const Ref<Scene>& context, EditorAssetManager* assetManager)
        : m_AssetManager(assetManager)
    {
        SetContext(context);
    }

    void InspectorPanel::SetContext(const Ref<Scene>& context)
    {
        m_Context = context;
    }

    void InspectorPanel::OnGUIRender(Entity selectedEntity) noexcept
    {
        ImGui::Begin(m_EditorDomain->Translate("inspector").c_str());

        if (selectedEntity == Entity::Null)
        {
            ImGui::End();
            return;
        }

        DrawComponents(selectedEntity);

        ImGui::End();
    }
    template <typename T, MType type>
    static void GetFieldData(MField& field, T* value, MObject* mObject, GameScriptField& gameScriptField)
    {
        if (mObject)
        {
            auto obj = mObject->GetFieldValue(field);
            *value = obj.GetValue<type>();
            return;
        }
        *value = gameScriptField.GetData<T>();
    }
    template <typename T>
    static void SetFieldData(MField& field, T* value, MObject* mObject, GameScriptField& gameScriptField)
    {
        if (mObject)
        {
            mObject->SetFieldValue(field, value);
            return;
        }
        gameScriptField.SetData<T>(*value);
    }

    void InspectorPanel::DrawComponents(Entity entity)
    {
        auto& tag = entity.GetComponent<TagComponent>().Tag;
        ImGui::InputText("##Tag", &tag);
        ImGui::SameLine();
        ImGui::PushItemWidth(-1);

        if (ImGui::Button("Add Component"))
        {
            ImGui::OpenPopup("AddComponent");
        }
        if (ImGui::BeginPopup("AddComponent"))
        {
            AddComponentPopup<CameraComponent>("Camera", entity);
            AddComponentPopup<SpriteRendererComponent>("Sprite", entity);
            AddComponentPopup<CircleRendererComponent>("Circle", entity);
            AddComponentPopup<MeshComponent>("Mesh", entity);
            AddComponentPopup<TextRendererComponent>("Text", entity);
            // AddComponentPopup<NativeScriptComponent>("Native Script", entity);
            AddComponentPopup<ScriptComponent>("Script", entity);
            AddComponentPopup<RigidBody2DComponent>("Rigid Body 2D", entity);
            AddComponentPopup<BoxCollider2DComponent>("Box Collider 2D", entity);
            ImGui::EndPopup();
        }

        ImGui::PopItemWidth();

        DrawConsistentComponentUI<TransformComponent>(
            m_EditorDomain->Translate("transform"),
            entity,
            [this](TransformComponent& transform)
            {
                DrawVec3ComponentUI(m_EditorDomain->Translate("transform.translation"), transform.Translation);
                glm::vec3 rotation = glm::degrees(transform.Rotation);
                DrawVec3ComponentUI(m_EditorDomain->Translate("transform.rotation"), rotation);
                transform.Rotation = glm::radians(rotation);
                DrawVec3ComponentUI(m_EditorDomain->Translate("transform.scale"), transform.Scale, 1.0f);
            });

        DrawComponentUI<CameraComponent>(
            m_EditorDomain->Translate("camera"),
            entity,
            [this](CameraComponent& camera)
            {
                static String projectionTypeStrings[] = {m_EditorDomain->Translate("camera.projection.perspective"),
                                                         m_EditorDomain->Translate("camera.projection.orthographic")};
                SceneCamera::CameraType currentProjectionType = camera.Camera.GetProjectionType();
                String& currentProjectionTypeString = projectionTypeStrings[static_cast<int>(currentProjectionType)];
                if (ImGui::BeginCombo(m_EditorDomain->Translate("camera.projection").c_str(),
                                      currentProjectionTypeString.c_str()))
                {
                    for (int i = 0; i < 2; ++i)
                    {
                        bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
                        if (ImGui::Selectable(projectionTypeStrings[i].c_str(), isSelected))
                        {
                            currentProjectionTypeString = projectionTypeStrings[i];
                            camera.Camera.SetProjectionType(static_cast<SceneCamera::CameraType>(i));
                        }
                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                if (currentProjectionType == SceneCamera::CameraType::Perspective)
                {
                    float fov = glm::degrees(camera.Camera.GetPerspectiveVerticalFOV());
                    if (ImGui::DragFloat(m_EditorDomain->Translate("camera.fov").c_str(), &fov, 0.1f))
                    {
                        camera.Camera.SetPerspectiveVerticalFOV(glm::radians(fov));
                    }
                    float nearClip = camera.Camera.GetPerspectiveNearClip();
                    if (ImGui::DragFloat(m_EditorDomain->Translate("camera.nearClip").c_str(), &nearClip, 0.1f))
                    {
                        camera.Camera.SetPerspectiveNearClip(nearClip);
                    }
                    float farClip = camera.Camera.GetPerspectiveFarClip();
                    if (ImGui::DragFloat(m_EditorDomain->Translate("camera.farClip").c_str(), &farClip, 0.1f))
                    {
                        camera.Camera.SetPerspectiveFarClip(farClip);
                    }
                }
                else if (camera.Camera.GetProjectionType() == SceneCamera::CameraType::Orthographic)
                {
                    float size = camera.Camera.GetOrthographicSize();
                    if (ImGui::DragFloat(m_EditorDomain->Translate("size").c_str(), &size, 0.1f))
                    {
                        camera.Camera.SetOrthographicSize(size);
                    }
                    float nearClip = camera.Camera.GetOrthographicNearClip();
                    if (ImGui::DragFloat(m_EditorDomain->Translate("camera.nearClip").c_str(), &nearClip, 0.1f))
                    {
                        camera.Camera.SetOrthographicNearClip(nearClip);
                    }
                    float farClip = camera.Camera.GetOrthographicFarClip();
                    if (ImGui::DragFloat(m_EditorDomain->Translate("camera.farClip").c_str(), &farClip, 0.1f))
                    {
                        camera.Camera.SetOrthographicFarClip(farClip);
                    }
                }
                ImGui::Checkbox(m_EditorDomain->Translate("camera.fixedAspectRatio").c_str(), &camera.FixedAspectRatio);
                bool oldPrimary = camera.Primary;
                ImGui::Checkbox(m_EditorDomain->Translate("primary", "gender", "female").c_str(), &camera.Primary);
                if (camera.Primary != oldPrimary && camera.Primary)
                {
                    auto cameras = m_Context->m_Registry.view<CameraComponent>();
                    for (auto cameraEntity : cameras)
                    {
                        m_Context->m_Registry.get<CameraComponent>(cameraEntity).Primary = false;
                    }
                    camera.Primary = true;
                }
            });

        DrawComponentUI<SpriteRendererComponent>(
            m_EditorDomain->Translate("sprite"),
            entity,
            [this](SpriteRendererComponent& sprite)
            {
                ImGui::ColorEdit4(m_EditorDomain->Translate("color").c_str(), sprite.Color.ValuePtr());

                String textureLabel;
                if (sprite.HasTexture)
                {
                    auto* texture = sprite.Texture(m_Project->GetProjectLocaleDomain().GetLocale());
                    float aspectRatio = (float)texture->GetWidth() / (float)texture->GetHeight();
                    if (ImGui::ImageButton((void*)texture->GetRendererID(),
                                           ImVec2(ImGui::GetContentRegionAvail().x * 0.95f,
                                                  ImGui::GetContentRegionAvail().x * 0.95f / aspectRatio),
                                           {0, 1},
                                           {1, 0}))
                    {
                        sprite.HasTexture = false;
                    }
                    auto& textureAsset = AssetManager::GetAsset<Asset>(sprite.TextureHandle);
                    if (textureAsset.GetType() == AssetType::Localized)
                    {
                        textureLabel =
                            String(textureAsset.Name) + " (" +
                            String(static_cast<LocalizedAsset&>(textureAsset)
                                       .GetAsset(m_Project->GetProjectLocaleDomain().GetLocale().GetLanguageString())
                                       .Name) +
                            ")";
                    }
                    else
                    {
                        textureLabel = textureAsset.Name;
                    }
                }
                else
                {
                    ImGui::Button(m_EditorDomain->Translate("texture").c_str(),
                                  ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
                }
                ImGui::AcceptDragAndDrop(
                    "CONTENT_BROWSER_ITEM",
                    [this, &sprite](void* data, size_t size)
                    {
                        Path texturePath = m_WorkingDirectory / static_cast<const char*>(data);
                        if (!ResourceManager::IsTexture2DExtension(texturePath.GetExtension()))
                            return;
                        auto name = texturePath.GetFileNameWithoutExtension().AsUTF8();
                        auto* handlePtr = m_AssetManager->GetAssetHandleByName(name);
                        if (!handlePtr)
                        {
                            m_AssetManager->LoadAsset(texturePath, {m_ProjectAssetRegistryID});
                            handlePtr = m_AssetManager->GetAssetHandleByName(name);
                        }
                        BeeCoreAssert(handlePtr, "Failed to load texture from path: {0}", texturePath.AsUTF8());
                        sprite.HasTexture = true;
                        sprite.TextureHandle = *handlePtr;
                    });

                ImGui::AcceptDragAndDrop<AssetHandle>("ASSET_BROWSER_TEXTURE2D_ITEM",
                                                      [this, &sprite](const auto& handle)
                                                      {
                                                          BeeExpects(m_AssetManager->IsAssetHandleValid(handle));
                                                          sprite.HasTexture = true;
                                                          sprite.TextureHandle = handle;
                                                      });

                if (sprite.HasTexture)
                {
                    ImGui::TextUnformatted(textureLabel.c_str());
                }

                ImGui::DragFloat(
                    m_EditorDomain->Translate("sprite.tilingFactor").c_str(), &sprite.TilingFactor, 0.1f, 0.0f, 100.0f);
            });

        DrawComponentUI<CircleRendererComponent>(
            m_EditorDomain->Translate("inspector.circleRenderer"),
            entity,
            [this](CircleRendererComponent& circle)
            {
                ImGui::ColorEdit4(m_EditorDomain->Translate("color").c_str(), circle.Color.ValuePtr());
                ImGui::DragFloat(m_EditorDomain->Translate("inspector.circleRenderer.thickness").c_str(),
                                 &circle.Thickness,
                                 0.025f,
                                 0.0f,
                                 1.0f);
                ImGui::DragFloat(m_EditorDomain->Translate("inspector.circleRenderer.fade").c_str(),
                                 &circle.Fade,
                                 0.0025f,
                                 0.005f,
                                 1.0f);
            });

        DrawComponentUI<MeshComponent>(
            m_EditorDomain->Translate("inspector.meshRenderer"),
            entity,
            [this](MeshComponent& meshComponent)
            {
                if (meshComponent.HasMeshes)
                {
                    if (ImGui::Button(meshComponent.MeshSource()->Name.data()))
                    {
                        meshComponent.HasMeshes = false;
                    }
                }
                else
                {
                    ImGui::Button(m_EditorDomain->Translate("none").c_str(), ImVec2(100.0f, 0.0f));
                }
                ImGui::AcceptDragAndDrop(
                    "CONTENT_BROWSER_ITEM",
                    [this, &meshComponent](void* data, size_t size)
                    {
                        Path meshSourcePath = m_WorkingDirectory / static_cast<const char*>(data);
                        if (!ResourceManager::IsMeshSourceExtension(meshSourcePath.GetExtension()))
                            return;
                        auto name = meshSourcePath.GetFileNameWithoutExtension().AsUTF8();
                        auto* handlePtr = m_AssetManager->GetAssetHandleByName(name);
                        if (!handlePtr)
                        {
                            m_AssetManager->LoadAsset(meshSourcePath, {m_ProjectAssetRegistryID});
                            handlePtr = m_AssetManager->GetAssetHandleByName(name);
                        }
                        BeeCoreAssert(handlePtr, "Failed to load mesh source from path: {0}", meshSourcePath.AsUTF8());
                        meshComponent.HasMeshes = true;
                        meshComponent.MeshSourceHandle = *handlePtr;
                    });

                ImGui::AcceptDragAndDrop<AssetHandle>("ASSET_BROWSER_MESHSOURCE_ITEM",
                                                      [this, &meshComponent](const auto& handle)
                                                      {
                                                          BeeExpects(m_AssetManager->IsAssetHandleValid(handle));
                                                          meshComponent.HasMeshes = true;
                                                          meshComponent.MeshSourceHandle = handle;
                                                      });

                // Material info
                bool upload = false;
                if (ImGui::ColorPicker4("Color", glm::value_ptr(meshComponent.MaterialInstance.data.colorFactors)))
                    upload = true;
                if (ImGui::DragFloat("Metalness",
                                     (float*)&meshComponent.MaterialInstance.data.metalRoughFactors.x,
                                     0.025f,
                                     0.0f,
                                     1.0f))
                    upload = true;
                if (ImGui::DragFloat("Roughness",
                                     (float*)&meshComponent.MaterialInstance.data.metalRoughFactors.y,
                                     0.025f,
                                     0.0f,
                                     1.0f))
                    upload = true;
                if (upload)
                    meshComponent.MaterialInstance.LoadData();
            });
        DrawComponentUI<TextRendererComponent>(
            m_EditorDomain->Translate("inspector.textRenderer"),
            entity,
            [this](TextRendererComponent& component)
            {
                ImGui::InputTextMultiline(m_EditorDomain->Translate("text").c_str(), &component.Text);
                auto& fontAsset = AssetManager::GetAsset<Asset>(component.FontHandle);
                String fontLabel;
                if (fontAsset.GetType() == AssetType::Localized)
                {
                    fontLabel =
                        String(fontAsset.Name) + " (" +
                        String(static_cast<LocalizedAsset&>(fontAsset)
                                   .GetAsset(m_Project->GetProjectLocaleDomain().GetLocale().GetLanguageString())
                                   .Name) +
                        ")";
                }
                else
                {
                    fontLabel = fontAsset.Name;
                }
                if (ImGui::Button(fontLabel.c_str(), ImVec2(0.0f, 0.0f)))
                {
                    component.FontHandle = EngineAssetRegistry::OpenSansRegular;
                }
                ImGui::AcceptDragAndDrop(
                    "CONTENT_BROWSER_ITEM",
                    [this, &component](void* data, size_t size)
                    {
                        Path fontPath = m_WorkingDirectory / static_cast<const char*>(data);
                        if (ResourceManager::IsFontExtension(fontPath.GetExtension()))
                        {
                            auto name = fontPath.GetFileNameWithoutExtension().AsUTF8();
                            auto* handlePtr = m_AssetManager->GetAssetHandleByName(name);
                            if (!handlePtr)
                            {
                                m_AssetManager->LoadAsset(fontPath, {m_ProjectAssetRegistryID});
                                handlePtr = m_AssetManager->GetAssetHandleByName(name);
                            }
                            BeeCoreAssert(handlePtr, "Failed to load font from path: {0}", fontPath.AsUTF8());
                            component.FontHandle = *handlePtr;
                        }
                    });
                ImGui::AcceptDragAndDrop<AssetHandle>("ASSET_BROWSER_FONT_ITEM",
                                                      [this, &component](const auto& handle)
                                                      {
                                                          BeeExpects(m_AssetManager->IsAssetHandleValid(handle));
                                                          component.FontHandle = handle;
                                                      });
                ImGui::ColorEdit4(m_EditorDomain->Translate("inspector.textRenderer.foreground").c_str(),
                                  component.Configuration.ForegroundColor.ValuePtr());
                // ImGui::ColorEdit4("Background", component.Configuration.BackgroundColor.ValuePtr());
                ImGui::DragFloat("Kerning offset", &component.Configuration.KerningOffset, 0.025f);
                ImGui::DragFloat("Line spacing", &component.Configuration.LineSpacing, 0.025f);
            });

        DrawComponentUI<ScriptComponent>(
            "Script",
            entity,
            [this, entity](ScriptComponent& script) mutable
            {
                std::vector<const char*> scriptNames;
                auto& scripts = ScriptingEngine::GetGameScripts();
                scriptNames.reserve(scripts.size() + 1);
                scriptNames.push_back("##");
                std::ranges::transform(scripts,
                                       std::back_inserter(scriptNames),
                                       [](auto& script) { return script.second->GetFullName().c_str(); });
                const char* currentScriptName = script.Class != nullptr ? script.Class->GetFullName().c_str() : "##";
                if (ImGui::BeginCombo("Class", currentScriptName))
                {
                    for (auto& scriptName : scriptNames)
                    {
                        bool isSelected = currentScriptName == scriptName;
                        if (ImGui::Selectable(scriptName, isSelected))
                        {
                            currentScriptName = scriptName;
                            if (!strcmp(currentScriptName, "##"))
                            {
                                script.SetClass(nullptr);
                            }
                            else
                            {
                                script.SetClass(&ScriptingEngine::GetGameScript(currentScriptName));
                            }
                        }
                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                if (!script.Class)
                    return;
                auto& fields = script.EditableFields;
                MObject* mObject = nullptr;
                if (m_Context->IsRuntime())
                {
                    auto* scriptInstance = ScriptingEngine::GetEntityScriptInstance(entity.GetUUID());
                    if (!scriptInstance)
                        return;
                    mObject = &scriptInstance->GetMObject();
                }
                for (auto& field : fields)
                {
                    auto& mField = field.GetMField();
                    auto* name = mField.GetName().c_str();
                    auto handleAssetField =
                        [mObject, name, &mField, this](AssetType assetType,
                                                       GameScriptField& field,
                                                       bool (*checkExtensionFunc)(const Path& extension))
                    {
                        AssetHandle value;
                        ImGui::Text(name);
                        ImGui::SameLine();
                        if (mObject)
                        {
                            auto object = mObject->GetFieldValue(mField);
                            ScriptingEngine::GetAssetHandle(object, value);
                        }
                        else
                        {
                            value = field.GetData<AssetHandle>();
                        }
                        bool isValid = AssetManager::IsAssetHandleValid(value);
                        String assetName;
                        if (isValid)
                        {
                            auto& asset = m_AssetManager->GetAssetMetadata(value);
                            if (asset.Type == AssetType::Localized)
                            {
                                assetName =
                                    String(asset.Name) + " (" +
                                    String(AssetManager::GetAsset<LocalizedAsset>(value)
                                               .GetAsset(
                                                   m_Project->GetProjectLocaleDomain().GetLocale().GetLanguageString())
                                               .Name) +
                                    ")";
                            }
                            else
                            {
                                assetName = asset.Name;
                            }
                        }
                        else
                        {
                            assetName = "null";
                        }
                        if (ImGui::Button(assetName.c_str()))
                        {
                            value = {0, 0};
                            if (!m_Context->IsRuntime())
                                SetFieldData(mField, &value, mObject, field);
                        }
                        if (m_Context->IsRuntime())
                            return;
                        if (ImGui::BeginDragDropTarget())
                        {
                            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                            {
                                Path assetPath = m_Project->FolderPath.get() / (const char*)payload->Data;
                                if (checkExtensionFunc(assetPath.GetExtension()))
                                {
                                    auto name = assetPath.GetFileNameWithoutExtension().AsUTF8();
                                    auto* handlePtr = m_AssetManager->GetAssetHandleByName(name);
                                    if (!handlePtr)
                                    {
                                        m_AssetManager->LoadAsset(assetPath, {m_Project->GetAssetRegistryID()});
                                    }
                                    handlePtr = m_AssetManager->GetAssetHandleByName(name);
                                    value = *handlePtr;
                                    SetFieldData(mField, &value, mObject, field);
                                }
                            }
                            ImGui::EndDragDropTarget();
                        }
                        ImGui::AcceptDragAndDrop<AssetHandle>(
                            AssetPanel::GetDragAndDropTypeName(assetType),
                            [this, &value, &mField, &mObject, &field](const auto& handle)
                            {
                                BeeExpects(m_AssetManager->IsAssetHandleValid(handle));
                                value = handle;
                                SetFieldData(mField, &value, mObject, field);
                            });
                    };
                    switch (mField.GetType())
                    {
                        case MType::Boolean:
                        {
                            bool value = false;
                            GetFieldData<bool, MType::Boolean>(mField, &value, mObject, field);
                            if (ImGui::Checkbox(name, &value))
                            {
                                SetFieldData(mField, &value, mObject, field);
                            }
                            break;
                        }
                        case MType::Int32:
                        {
                            int32_t value = 0;
                            GetFieldData<int32_t, MType::Int32>(mField, &value, mObject, field);
                            if (ImGui::DragInt(name, &value))
                            {
                                SetFieldData(mField, &value, mObject, field);
                            }
                            break;
                        }
                        case MType::Single:
                        {
                            float value = 0;
                            GetFieldData<float, MType::Single>(mField, &value, mObject, field);
                            if (ImGui::DragFloat(name, &value))
                            {
                                SetFieldData(mField, &value, mObject, field);
                            }
                            break;
                        }
                        /*case MType::String:
                        {
                            std::string value;
                            mObject.GetFieldValue(*field, &value);
                            char buffer[256];
                            strcpy(buffer, value.c_str());
                            if(ImGui::InputText(name.data(), buffer, 256))
                            {
                                mObject.SetFieldValue(*field, buffer);
                            }
                            break;
                        }*/
                        case MType::Vector2:
                        {
                            glm::vec2 value;
                            GetFieldData<glm::vec2, MType::Vector2>(mField, &value, mObject, field);
                            if (ImGui::DragFloat2(name, glm::value_ptr(value)))
                            {
                                SetFieldData(mField, &value, mObject, field);
                            }
                            break;
                        }
                        case MType::Vector3:
                        {
                            glm::vec3 value;
                            GetFieldData<glm::vec3, MType::Vector3>(mField, &value, mObject, field);
                            if (ImGui::DragFloat3(name, glm::value_ptr(value)))
                            {
                                SetFieldData(mField, &value, mObject, field);
                            }
                            break;
                        }
                        case MType::Vector4:
                        {
                            glm::vec4 value;
                            GetFieldData<glm::vec4, MType::Vector4>(mField, &value, mObject, field);
                            if (ImGui::DragFloat4(name, glm::value_ptr(value)))
                            {
                                SetFieldData(mField, &value, mObject, field);
                            }
                            break;
                        }
                        case MType::Color:
                        {
                            Color4 value;
                            GetFieldData<Color4, MType::Color>(mField, &value, mObject, field);
                            if (ImGui::ColorEdit4(name, value.ValuePtr()))
                            {
                                SetFieldData(mField, &value, mObject, field);
                            }
                            break;
                        }
                        case MType::Asset:
                        {
                            AssetHandle value;
                            ImGui::Text(name);
                            ImGui::SameLine();
                            if (mObject)
                            {
                                auto object = mObject->GetFieldValue(mField);
                                ScriptingEngine::GetAssetHandle(object, value);
                            }
                            else
                            {
                                value = field.GetData<AssetHandle>();
                            }
                            bool isValid = AssetManager::IsAssetHandleValid(value);
                            if (ImGui::Button(isValid ? AssetManager::GetAsset<Asset>(value).Name.data() : "null"))
                            {
                                value = {0, 0};
                                if (!m_Context->IsRuntime())
                                    SetFieldData(mField, &value, mObject, field);
                            }
                            if (m_Context->IsRuntime())
                                break;
                            if (ImGui::BeginDragDropTarget())
                            {
                                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                                {
                                    Path assetPath = m_Project->FolderPath.get() / (const char*)payload->Data;

                                    if (ResourceManager::IsAssetExtension(assetPath.GetExtension()))
                                    {
                                        auto name = assetPath.GetFileNameWithoutExtension().AsUTF8();
                                        auto* handlePtr = m_AssetManager->GetAssetHandleByName(name);
                                        if (!handlePtr)
                                        {
                                            m_AssetManager->LoadAsset(assetPath, {m_Project->GetAssetRegistryID()});
                                        }
                                        handlePtr = m_AssetManager->GetAssetHandleByName(name);
                                        value = *handlePtr;
                                        SetFieldData(mField, &value, mObject, field);
                                    }
                                }
                                ImGui::EndDragDropTarget();
                            }
                            ImGui::AcceptDragAndDrop<AssetHandle>(
                                "ASSET_BROWSER_FONT_ITEM",
                                [this, &value, &mField, &mObject, &field](const auto& handle)
                                {
                                    BeeExpects(m_AssetManager->IsAssetHandleValid(handle));
                                    value = handle;
                                    SetFieldData(mField, &value, mObject, field);
                                });
                            ImGui::AcceptDragAndDrop<AssetHandle>(
                                "ASSET_BROWSER_TEXTURE2D_ITEM",
                                [this, &value, &mField, &mObject, &field](const auto& handle)
                                {
                                    BeeExpects(m_AssetManager->IsAssetHandleValid(handle));
                                    value = handle;
                                    SetFieldData(mField, &value, mObject, field);
                                });
                            break;
                        }
                        case MType::Texture2D:
                        {
                            handleAssetField(AssetType::Texture2D, field, ResourceManager::IsTexture2DExtension);
                            break;
                        }
                        case MType::Font:
                        {
                            handleAssetField(AssetType::Font, field, ResourceManager::IsFontExtension);
                            break;
                        }
                        case MType::Prefab:
                        {
                            handleAssetField(AssetType::Prefab, field, ResourceManager::IsPrefabExtension);
                            break;
                        }
                        case MType::Scene:
                        {
                            handleAssetField(AssetType::Scene, field, ResourceManager::IsSceneExtension);
                            break;
                        }
                    }
                }
            });

        DrawComponentUI<RigidBody2DComponent>(
            "Rigid Body 2D",
            entity,
            [this](RigidBody2DComponent& component)
            {
                constexpr static const char* bodyTypeStrings[] = {"Static", "Dynamic", "Kinematic"};
                const char* currentBodyTypeString = bodyTypeStrings[static_cast<int>(component.Type)];
                if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
                {
                    for (int i = 0; i < 2; ++i)
                    {
                        bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
                        if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
                        {
                            currentBodyTypeString = bodyTypeStrings[i];
                            component.Type = static_cast<RigidBody2DComponent::BodyType>(i);
                        }
                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
            });
        DrawComponentUI<BoxCollider2DComponent>(
            "Box Collider 2D",
            entity,
            [this](auto& component)
            {
                constexpr static const char* typeStrings[] = {"Box", "Circle"};
                const char* currentTypeString = typeStrings[static_cast<int>(component.Type)];
                if (ImGui::BeginCombo("Collider Type", currentTypeString))
                {
                    for (int i = 0; i < 2; ++i)
                    {
                        bool isSelected = currentTypeString == typeStrings[i];
                        if (ImGui::Selectable(typeStrings[i], isSelected))
                        {
                            currentTypeString = typeStrings[i];
                            component.Type = static_cast<BoxCollider2DComponent::ColliderType>(i);
                        }
                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
                ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
                ImGui::DragFloat("Density", &component.Density, 0.1f, 0.0f, 1.0f);
                ImGui::DragFloat("Friction", &component.Friction, 0.1f, 0.0f, 1.0f);
                ImGui::DragFloat("Restitution", &component.Restitution, 0.1f, 0.0f, 1.0f);
                ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.1f, 0.0f);
            });

        DrawComponentUI<NativeScriptComponent>("Native Script",
                                               entity,
                                               [this](NativeScriptComponent& script)
                                               {
                                                   std::vector<const char*> scriptNames;
                                                   if (m_NativeScripts == nullptr)
                                                   {
                                                       m_NativeScripts =
                                                           &NativeScriptFactory::GetInstance().GetNativeScripts();
                                                   }
                                                   scriptNames.reserve(m_NativeScripts->size() + 1);
                                                   scriptNames.push_back("##");
                                                   for (auto& script : *m_NativeScripts)
                                                   {
                                                       scriptNames.push_back(script.Name.c_str());
                                                   }
                                                   const char* currentScriptName =
                                                       script.Name != "" ? script.Name.c_str() : "##";
                                                   if (ImGui::BeginCombo("Script", currentScriptName))
                                                   {
                                                       for (auto& scriptName : scriptNames)
                                                       {
                                                           bool isSelected = currentScriptName == scriptName;
                                                           if (ImGui::Selectable(scriptName, isSelected))
                                                           {
                                                               currentScriptName = scriptName;
                                                               script.Name = currentScriptName;
                                                           }
                                                           if (isSelected)
                                                           {
                                                               ImGui::SetItemDefaultFocus();
                                                           }
                                                       }
                                                       ImGui::EndCombo();
                                                   }
                                               });
    }

    void
    InspectorPanel::DrawVec3ComponentUI(const String& label, glm::vec3& values, float resetValue, float columnWidth)
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
        if (ImGui::Button("X", buttonSize))
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
        if (ImGui::Button("Y", buttonSize))
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
        if (ImGui::Button("Z", buttonSize))
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

    template <typename T>
    void InspectorPanel::AddComponentPopup(const String& label, Entity entity)
    {
        if (entity.HasComponent<T>())
            return;
        if (ImGui::MenuItem(label.data()))
        {
            entity.AddComponent<T>();
            ImGui::CloseCurrentPopup();
        }
    }

    template <typename T, typename UIFunction>
    void InspectorPanel::DrawConsistentComponentUI(const String& label, Entity entity, UIFunction func)
    {
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                                 ImGuiTreeNodeFlags_SpanAvailWidth |
                                                 ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

        if (m_Context->m_Registry.all_of<T>(entity))
        {
            auto& component = m_Context->m_Registry.get<T>(entity);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
            ImGui::Separator();
            bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, label.data());
            ImGui::PopStyleVar();
            if (open)
            {
                func(component);
                ImGui::TreePop();
            }
        }
    }

    template <typename T, typename UIFunction>
    void InspectorPanel::DrawComponentUI(const String& label, Entity entity, UIFunction func)
    {
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                                 ImGuiTreeNodeFlags_SpanAvailWidth |
                                                 ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

        if (m_Context->m_Registry.all_of<T>(entity))
        {
            auto& component = m_Context->m_Registry.get<T>(entity);

            ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImGui::Separator();
            bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, label.data());
            ImGui::PopStyleVar();
            ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
            if (ImGui::Button("...", ImVec2{lineHeight, lineHeight}))
            {
                ImGui::OpenPopup("ComponentSettings");
            }

            bool removeComponent = false;
            if (ImGui::BeginPopup("ComponentSettings"))
            {
                if (ImGui::MenuItem("Remove Component"))
                {
                    removeComponent = true;
                }
                ImGui::EndPopup();
            }

            if (open)
            {
                func(component);
                ImGui::TreePop();
            }

            if (removeComponent)
            {
                m_Context->m_Registry.remove<T>(entity);
            }
        }
    }
} // namespace BeeEngine::Editor
