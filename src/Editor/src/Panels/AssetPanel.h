//
// Created by alexl on 19.08.2023.
//

#pragma once

#include "AssetEditPanel.h"
#include "Core/AssetManagement/EditorAssetManager.h"
#include "Locale/Locale.h"
#include "ProjectFile.h"

namespace BeeEngine::Editor
{
    class AssetPanel
    {
    public:
        AssetPanel(EditorAssetManager* assetManager, Locale::Domain& domain);
        void SetProject(ProjectFile* project);

        void SetAssetDeletedCallback(const std::function<void(AssetHandle)>& callback) { m_OnAssetDeleted = callback; }

        void Render();

        static const char* GetDragAndDropTypeName(AssetType type);

    private:
        Scope<AssetEditPanel> m_AssetEditPanel;
        EditorAssetManager* m_AssetManager;
        Locale::Domain* m_EditorDomain;
        ProjectFile* m_Project;
        std::function<void(AssetHandle)> m_OnAssetDeleted;
    };
} // namespace BeeEngine::Editor
