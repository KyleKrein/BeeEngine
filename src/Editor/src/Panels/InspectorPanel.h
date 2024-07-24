//
// Created by alexl on 04.06.2023.
//

#pragma once

#include "BeeEngine.h"
#include "Core/AssetManagement/EditorAssetManager.h"
#include "ProjectFile.h"

namespace BeeEngine::Editor
{
    class InspectorPanel
    {
    public:
        InspectorPanel(Property<Scope<ProjectFile>>& project,
                       EditorAssetManager* assetManager,
                       Locale::Domain& editorDomain)
            : m_AssetManager(assetManager), m_EditorDomain(&editorDomain)
        {
            project.valueChanged().connect(
                [this](const auto& newProject)
                {
                    m_Project = newProject.get();
                    m_ProjectAssetRegistryID = newProject->GetAssetRegistryID();
                    m_WorkingDirectory = newProject->FolderPath.get();
                });
        }
        explicit InspectorPanel(const Ref<Scene>& context, EditorAssetManager* assetManager);

        void SetContext(const Ref<Scene>& context);

        void OnGUIRender(Entity selectedEntity) noexcept;

    private:
        UUID m_ProjectAssetRegistryID;
        Ref<Scene> m_Context;
        const ProjectFile* m_Project = nullptr;
        EditorAssetManager* m_AssetManager = nullptr;

        Path m_WorkingDirectory;
        Locale::Domain* m_EditorDomain = nullptr;

        const std::vector<NativeScriptInfo>* m_NativeScripts = nullptr;

        void DrawComponents(Entity entity);

        template <typename T, typename UIFunction>
        void DrawComponentUI(const String& label, Entity entity, UIFunction func);
        template <typename T, typename UIFunction>
        void DrawConsistentComponentUI(const String& label, Entity entity, UIFunction func);

        template <typename T>
        void AddComponentPopup(const String& label, Entity entity);
        void DrawVec3ComponentUI(const String& label,
                                 glm::vec3& values,
                                 float resetValue = 0.0f,
                                 float columnWidth = 100.0f);
    };
} // namespace BeeEngine::Editor
