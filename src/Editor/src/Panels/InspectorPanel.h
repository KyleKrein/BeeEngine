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
        InspectorPanel(EditorAssetManager* assetManager, Locale::Domain& editorDomain)
            : m_AssetManager(assetManager), m_EditorDomain(&editorDomain)
        {
        }
        explicit InspectorPanel(const Ref<Scene>& context, EditorAssetManager* assetManager);

        void SetProject(ProjectFile* project) { m_Project = project; }

        void SetContext(const Ref<Scene>& context);
        void SetProjectAssetRegistryID(UUID id) { m_ProjectAssetRegistryID = id; }

        void OnGUIRender(Entity selectedEntity) noexcept;

        void SetWorkingDirectory(const Path& path) noexcept { m_WorkingDirectory = path; }

    private:
        UUID m_ProjectAssetRegistryID;
        Ref<Scene> m_Context;
        ProjectFile* m_Project = nullptr;
        EditorAssetManager* m_AssetManager = nullptr;

        Path m_WorkingDirectory;
        Locale::Domain* m_EditorDomain = nullptr;

        const std::vector<NativeScriptInfo>* m_NativeScripts = nullptr;

        void DrawComponents(Entity entity);

        template <typename T, typename UIFunction>
        void DrawComponentUI(std::string_view label, Entity entity, UIFunction func);
        template <typename T, typename UIFunction>
        void DrawConsistentComponentUI(std::string_view label, Entity entity, UIFunction func);

        template <typename T>
        void AddComponentPopup(std::string_view label, Entity entity);
        void DrawVec3ComponentUI(std::string_view label,
                                 glm::vec3& values,
                                 float resetValue = 0.0f,
                                 float columnWidth = 100.0f);
    };
} // namespace BeeEngine::Editor
